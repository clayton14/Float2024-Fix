idf_component_register(SRCS 
                        "webserver.c" 
                        "stepper.c"
                        "main.c"
                        INCLUDE_DIRS "./include"
                        EMBED_TXTFILES "./web/index.html"
                        EMBED_TXTFILES "./web/test.js"
                    )


# set(EMBED_TXTFILES, "./web/index_float.html")
# set(COMPONENT_ADD_INCLUDEDIRS "./include")