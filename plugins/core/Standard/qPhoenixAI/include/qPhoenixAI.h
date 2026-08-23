#pragma once

#include "ccStdPluginInterface.h"

class qPhoenixAI final : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES( ccPluginInterface ccStdPluginInterface )
	Q_PLUGIN_METADATA( IID "cccorp.cloudcompare.plugin.qPhoenixAI" FILE "../info.json" )

public:
	explicit qPhoenixAI( QObject* parent = nullptr );
	QList<QAction*> getActions() override;

private:
	QAction* m_openAssistant = nullptr;
};
