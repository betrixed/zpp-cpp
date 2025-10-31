#ifndef WCX_XMLPARSE_CPP
#define WCX_XMLPARSE_CPP

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef WCC_XML_PARSE_H
#include "xmlparse.h"
#endif

constexpr std::string_view kCDATAi = "<![CDATA[";
constexpr std::string_view kCOMMENTi = "<!--";

constexpr std::string_view kCDATAf = "[CDATA[";
constexpr std::string_view kDASH2f = "--";

constexpr std::string_view kCDATA_END = "]>";

constexpr std::string_view kVERSION = "version";
constexpr std::string_view kDOCTYPE = "DOCTYPE";

constexpr std::string_view kENCODING = "encoding";
constexpr std::string_view kSTANDALONE = "standalone";

constexpr std::string_view LT_ent = "lt";
constexpr std::string_view GT_ent = "gt";
constexpr std::string_view AMP_ent = "amp";
constexpr std::string_view QUOT_ent = "quot";
constexpr std::string_view APOS_ent = "apos";

namespace wcx {

	using namespace zpp;



class XMLP_init : public state_init {
public:
	str_intern LT_ent;
	str_intern GT_ent;
	str_intern AMP_ent;
	str_intern QUOT_ent;
	str_intern APOS_ent;

	void init() override;
};

void 
XMLP_init::init()
{
	 LT_ent = "lt";
	 GT_ent = "gt";
	 AMP_ent = "amp";
	 QUOT_ent = "quot";
	 APOS_ent = "apos";
};

XMLP_init XMLPi;

}; // namespace wcx

#include "xmlchar.cpp"
#include "itemreturn.cpp"
#include "errorstack.cpp"
#include "parsecontext.cpp"
#include "xmlparse.cpp"
#include "coreparse.cpp"

#endif