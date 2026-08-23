#include "qPhoenixAI.h"
#include "ccMainAppInterface.h"

#include <QAction>

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace
{
class PhoenixDialog final : public QDialog
{
public:
	explicit PhoenixDialog(QWidget* parent = nullptr)
		: QDialog(parent)
	{
		setWindowTitle(tr("Phoenix AI — safe plan preview"));
		resize(620, 470);

		auto* layout = new QVBoxLayout(this);
		layout->addWidget(new QLabel(tr("Describe the 3D operation. Phoenix returns a plan only; it cannot modify data in this first release."), this));
		m_prompt = new QPlainTextEdit(this);
		m_prompt->setPlaceholderText(tr("Example: Remove isolated noise and prepare a volume-measurement workflow."));
		layout->addWidget(m_prompt);

		auto* form = new QFormLayout;
		m_endpoint = new QLineEdit(QStringLiteral("http://127.0.0.1:8765/v1/plans"), this);
		form->addRow(tr("Local Phoenix endpoint:"), m_endpoint);
		layout->addLayout(form);

		auto* preview = new QPushButton(tr("Request safe preview"), this);
		layout->addWidget(preview);
		m_result = new QPlainTextEdit(this);
		m_result->setReadOnly(true);
		m_result->setPlaceholderText(tr("The reviewed processing plan will appear here."));
		layout->addWidget(m_result);
		layout->addWidget(new QLabel(tr("Privacy: only the command text is sent. This plugin sends no point-cloud data."), this));
		auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
		connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
		layout->addWidget(buttons);
		connect(preview, &QPushButton::clicked, this, [this] { requestPreview(); });
	}

private:
	void requestPreview()
	{
		const QString prompt = m_prompt->toPlainText().trimmed();
		if (prompt.isEmpty())
		{
			QMessageBox::information(this, tr("Phoenix AI"), tr("Enter an operation before requesting a preview."));
			return;
		}
		const QUrl endpoint = QUrl::fromUserInput(m_endpoint->text());
		const QString host = endpoint.host().toLower();
		if (!endpoint.isValid() || endpoint.scheme() != QStringLiteral("http")
			|| (host != QStringLiteral("127.0.0.1") && host != QStringLiteral("localhost") && host != QStringLiteral("::1")))
		{
			QMessageBox::warning(this, tr("Phoenix AI"), tr("Use a valid loopback HTTP endpoint."));
			return;
		}
		m_result->setPlainText(tr("Requesting a safe plan…"));
		QNetworkRequest request(endpoint);
		request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
		QJsonObject payload{{QStringLiteral("prompt"), prompt}, {QStringLiteral("mode"), QStringLiteral("preview")}};
		auto* reply = m_network.post(request, QJsonDocument(payload).toJson(QJsonDocument::Compact));
		connect(reply, &QNetworkReply::finished, this, [this, reply]
		{
			const QByteArray response = reply->readAll();
			if (reply->error() != QNetworkReply::NoError)
				m_result->setPlainText(tr("Phoenix service unavailable: %1\n\nStart a compatible local service, then try again.").arg(reply->errorString()));
			else
				m_result->setPlainText(QString::fromUtf8(response));
			reply->deleteLater();
		});
	}

	QPlainTextEdit* m_prompt = nullptr;
	QLineEdit* m_endpoint = nullptr;
	QPlainTextEdit* m_result = nullptr;
	QNetworkAccessManager m_network;
};
}

qPhoenixAI::qPhoenixAI(QObject* parent)
	: QObject(parent)
	, ccStdPluginInterface(":/CC/plugin/qPhoenixAI/info.json")
{
}

QList<QAction*> qPhoenixAI::getActions()
{
	if (!m_openAssistant)
	{
		m_openAssistant = new QAction(tr("Phoenix AI plan preview"), this);
		m_openAssistant->setToolTip(tr("Ask Phoenix AI to prepare a reviewed, non-destructive 3D workflow"));
		connect(m_openAssistant, &QAction::triggered, this, [this]
		{
			PhoenixDialog dialog(m_app ? m_app->getMainWindow() : nullptr);
			dialog.exec();
		});
	}
	return {m_openAssistant};
}
