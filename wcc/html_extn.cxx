#ifndef HTML_EXTN_H
#define HTML_EXTN_H


// compile 
#include "wcc/assets.cpp"
#include "wcc/htmlgem.cpp"
#include "wcc/htmlplates.cpp"
#include "wcc/money_fmt.cpp"
#include "wcc/plate.cpp"
#include "wcc/plate_engine.cpp"
#include "wcc/search_list.cpp"
#include "md4c/markhtml.cpp"

void register_html_extn(INIT_FUNC_ARGS)
{

#ifdef WCC_ASSETS_CPP
	PHP_MINIT(wcc_assets_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef HTMLGEM_CPP
	PHP_MINIT(Wcc_HtmlGem_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef MONEY_FMT_CPP
	PHP_MINIT(Wcc_Money_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef PLATE_ENGINE_CPP
	PHP_MINIT(SearchList_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(Wcc_PlateEngine_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(Wcc_Plate_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(Wcc_HtmlPlates_reg)(INIT_FUNC_ARGS_PASSTHRU);	
#endif

#  ifdef MARKHTML_CPP
	PHP_MINIT(Wcc_MarkToHtml_reg)(INIT_FUNC_ARGS_PASSTHRU);
#  endif
}

#endif
