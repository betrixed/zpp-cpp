#ifndef REQUEST_GLOBALS_H
#define REQUEST_GLOBALS_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef WCC_HMAP_H
#include "hmap.h"
#endif

//request_globals.h

namespace wcc {
	using namespace zpp;
	
	enum Upload {
	    ERROR_OK   = 0,
	    ERROR_UPMAX = 1,
	    ERROR_FILEMAX = 2
	};

	class RequestGlobals : public base_d {
	protected:
		// Hmap object managed arrays 
		obj_rc server_;
		obj_rc get_;
		
		obj_rc files_;

		obj_rc request_;
		obj_rc post_;

		str_rc host_;
		str_rc body_;

		int  verb_;
		bool strictHost_;
		bool methodOverride_;
		bool spoof_;
		
		val_rc authCallback_;

	// hidden methods
		int fileCounter(htab_rd data, bool onlySuccessful);
		str_rc getBestQuality(htab_rd parts, str_ptr name);

		htab_rc getQualityHeader(str_ptr key, str_ptr name);
		htab_rc resolveAuthorizationHeaders();

		htab_rc smoothFiles(htab_rd names, htab_rd types,
			htab_rd tmp_names, htab_rd sizes,
			htab_rd errors, str_ptr prefix);

		htab_rd readServer();

	public:

		enum {
			PRE_AUTHORIZE, POST_AUTHORIZE
		};
		
		static base_obj_mgr<RequestGlobals> omg;

		virtual void debug_info(htab_rw di);
		
		void construct();

		obj_ptr post();
		obj_ptr query();
		obj_ptr server();

		htab_rd getPost();
		htab_rd getQuery();
		htab_rd getServer();

		obj_rc makeFile(htab_rd data, str_ptr name);

		void setAuthCallback(val_ptr callback);

		htab_rc getAcceptableContent();
		htab_rc getBasicAuth();
		str_rc getBestAccept();
		str_rc getBestCharset();

		str_rc getBestLanguage();
		str_rc getBestQuality();

		str_rc getClientAddress(bool trustHeader = false);

		htab_rc getClientCharsets();

		str_rc getContentType();

		htab_rc getDigestAuth();

		str_rc getHTTPReferer();

		val_rc getHeader(str_ptr header);

		void setHeader(str_ptr header, val_ptr value);
		
		htab_rc getHeaders();

		str_rc getHttpHost();

		val_rc getJsonRawBody(bool asArray);

		htab_rc getLanguages();

		int getMethod();

		bool getMethodOverride();

		void setMethodOverride(bool value);

		int getPort();


		
		str_rc getRawBody();

		str_ptr getScheme();

		str_rc getServerAddress();

		str_rc getServerName();

		bool getSpoofMethod();

		void setSpoofMethod(bool val);

		str_rc getURI(bool onlyPath = false);

		htab_rc getUploadedFiles(bool onlySuccess = false, bool namekeys = false);

		str_rc getUserAgent();

		bool hasFiles();

		bool hasHeader(str_ptr header);

		bool hasQuery(str_ptr key);

		bool hasRequest(str_ptr key);

		bool isAjax();

		bool isMethod(val_ptr methods, bool strict = false);

		bool isOptions();

		bool isPatch();

		bool isPost();

		bool isPurge();

		bool isPut();

		bool isSecure();

		bool isSoap();

		int numFiles(bool onlySuccess);

		void setStrictHost(bool val);
	};


	class request_init : public state_init {
	public:
		zstr_intern	   PHP_AUTH_USER;
		zstr_intern	   Php_Auth_User;

		zstr_intern	   PHP_AUTH_PW;
		zstr_intern	   Php_Auth_Pw;

		zstr_intern    PHP_AUTH_DIGEST;
		zstr_intern    Php_Auth_Digest;


		zstr_intern	   HTTP_AUTHORIZATION;
		zstr_intern	   REDIRECT_HTTP_AUTHORIZATION;

		zstr_intern    Authorization;
		
		zstr_intern    key_key;
		
		zstr_intern	   tmp_name;
		zstr_intern	   size_key;
		zstr_intern	   error_key;
		zstr_intern	   accept;
		zstr_intern	   username;
		zstr_intern	   password;
		zstr_intern	   language;
		zstr_intern    charset;
		zstr_intern    quality;
		zstr_intern    namekey;
		zstr_intern    typekey;
		zstr_intern    ext_key;
		zstr_intern    realtype_key;
		zstr_intern    equals_key;

		zstr_intern    basic_sp;
		zstr_intern    bearer_sp;
		zstr_intern    digest_sp;
		zstr_intern    step_key;
		zstr_intern    server_key;
		zstr_intern    colon_key;
		zstr_intern    headers_key;

		zstr_intern    get_key;
		zstr_intern    post_key;
		zstr_intern    request_key;
		zstr_intern    files_key;
		zstr_intern    spoof_key;
		zstr_intern    verb_key;
		zstr_intern    strict_host;
		zstr_intern    method_override;

		zstr_intern    G_SERVER;
		zstr_intern    G_GET;
		zstr_intern    G_REQUEST;
		zstr_intern    G_POST;
		zstr_intern    G_FILES;

		zstr_intern    HTTP_ACCEPT;
		zstr_intern    HTTP_X_FORWARDED_FOR;
		zstr_intern    HTTP_CLIENT_IP;
		zstr_intern    REMOTE_ADDR;
		zstr_intern    comma_key;
		zstr_intern    HTTP_ACCEPT_CHARSET;
		zstr_intern    CONTENT_TYPE;
		zstr_intern    CONTENT_LENGTH;
		zstr_intern    CONTENT_MD5; 

		zstr_intern    HTTP_REFERER;

		zstr_intern    hyphen_key;
		zstr_intern    underscore;
		zstr_intern    blank_key;
		zstr_intern    HTTP_;
		zstr_intern    HTTP_HOST;
		zstr_intern    SERVER_NAME;
		zstr_intern    SERVER_ADDR;
		zstr_intern    SERVER_PORT;
		zstr_intern    REQUEST_URI;

		zstr_intern    HTTP_ACCEPT_LANGUAGE;
		zstr_intern    REQUEST_METHOD;
		zstr_intern    X_HTTP_METHOD_OVERRIDE;


		zstr_intern    _method;
		zstr_intern    php_input;
		zstr_intern    HTTPS;
		zstr_intern    https;
		zstr_intern    http;
		zstr_intern    off_key;
		zstr_intern    localhost;

		zstr_intern    HTTP_X_REQUESTED_WITH;
		zstr_intern    HTTP_USER_AGENT;
		zstr_intern    XMLHttpRequest;
		zstr_intern    HTTP_SOAPACTION;
		zstr_intern    soap_mime;

		zstr_intern    gethostbyname;
		zstr_intern    finfo_open;
		zstr_intern    finfo_file;
		zstr_intern    finfo_close;	
		zstr_intern    is_uploaded_file;
		zstr_intern    move_uploaded_file;
		


		void init() override;
	};

	extern request_init RQit;


	str_rc gethostbyname(str_ptr str);
    val_rc finfo_open(int infoflags);
    val_rc finfo_file(val_rc& finfo, str_ptr path);
    val_rc finfo_close(val_rc& finfo);
    bool is_uploaded_file(str_ptr path);
    bool move_uploaded_file(str_ptr from, str_ptr to);
};

#endif