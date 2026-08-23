# Phoenix AI is deliberately optional: it communicates with a local service
# and never embeds an LLM or copies code from an external agent.
option( PLUGIN_PHOENIX_AI "Build the Phoenix AI CloudCompare plugin" ON )

if( PLUGIN_PHOENIX_AI )
	project( qPhoenixAI )
	AddPlugin( NAME ${PROJECT_NAME} )
	find_package( Qt6 REQUIRED COMPONENTS Network Widgets )
	target_link_libraries( ${PROJECT_NAME} Qt6::Network Qt6::Widgets )
	add_subdirectory( include )
	add_subdirectory( src )
endif()
