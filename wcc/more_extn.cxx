#ifndef MORE_EXTN_CXX
#define MORE_EXTN_CXX

#include "toml/toml_php.cpp"
#include "tinyxml/dxmlread.cpp"

void register_more_extn(INIT_FUNC_ARGS)
{
# ifdef DAYTIME_CPP
	PHP_MINIT(Wcc_Day24_reg)(INIT_FUNC_ARGS_PASSTHRU);
# endif

# ifdef TOML_PHP_CPP
	PHP_MINIT(Wcc_Toml_reg)(INIT_FUNC_ARGS_PASSTHRU);
# endif
	
# ifdef DXMLREAD_CPP
	PHP_MINIT(Wcc_XmlRead_reg)(INIT_FUNC_ARGS_PASSTHRU);
# endif
}


#endif