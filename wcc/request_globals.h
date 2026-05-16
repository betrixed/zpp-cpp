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

		obj_rc globals_;

		str_rc host_;
		str_rc body_;

		int  verb_;
		bool strictHost_;
		bool methodOverride_;
		bool spoof_;
		
		val_rc authCallback_;

	// hidden methods
		int fileCounter(htab_ptr data, bool onlySuccessful);
		str_rc getBestQuality(htab_ptr parts, str_ptr name);

		htab_rc getQualityHeader(str_ptr key, str_ptr name);
		htab_rc resolveAuthorizationHeaders();

		htab_rc smoothFiles(htab_ptr names, htab_ptr types,
			htab_ptr tmp_names, htab_ptr sizes,
			htab_ptr errors, str_ptr prefix);

		htab_ptr readServer();

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

		htab_ptr getPost();
		htab_ptr getQuery();
		htab_ptr getServer();

		obj_rc makeFile(htab_ptr data, str_ptr name);

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
		str_intern	   PHP_AUTH_USER;
		str_intern	   Php_Auth_User;

		str_intern	   PHP_AUTH_PW;
		str_intern	   Php_Auth_Pw;

		str_intern    PHP_AUTH_DIGEST;
		str_intern    Php_Auth_Digest;


		str_intern	   HTTP_AUTHORIZATION;
		str_intern	   REDIRECT_HTTP_AUTHORIZATION;

		str_intern    Authorization;
		
		str_intern    key_key;
		
		str_intern	   tmp_name;
		str_intern	   size_key;
		str_intern	   error_key;
		str_intern	   accept;
		str_intern	   username;
		str_intern	   password;
		str_intern	   language;
		str_intern    charset;
		str_intern    quality;
		str_intern    namekey;
		str_intern    typekey;
		str_intern    ext_key;
		str_intern    realtype_key;
		str_intern    equals_key;

		str_intern    basic_sp;
		str_intern    bearer_sp;
		str_intern    digest_sp;
		str_intern    step_key;
		str_intern    server_key;
		str_intern    colon_key;
		str_intern    headers_key;

		str_intern    get_key;
		str_intern    post_key;
		str_intern    request_key;
		str_intern    files_key;
		str_intern    spoof_key;
		str_intern    verb_key;
		str_intern    strict_host;
		str_intern    method_override;

		str_intern    G_SERVER;
		str_intern    G_GET;
		str_intern    G_REQUEST;
		str_intern    G_POST;
		str_intern    G_FILES;
		str_intern    G_SESSION;
		str_intern    G_GLOBALS;

		str_intern    HTTP_ACCEPT;
		str_intern    HTTP_X_FORWARDED_FOR;
		str_intern    HTTP_CLIENT_IP;
		str_intern    REMOTE_ADDR;
		str_intern    comma_key;
		str_intern    HTTP_ACCEPT_CHARSET;
		str_intern    CONTENT_TYPE;
		str_intern    CONTENT_LENGTH;
		str_intern    CONTENT_MD5; 

		str_intern    HTTP_REFERER;

		str_intern    hyphen_key;
		str_intern    underscore;
		str_intern    blank_key;
		str_intern    HTTP_;
		str_intern    HTTP_HOST;
		str_intern    SERVER_NAME;
		str_intern    SERVER_ADDR;
		str_intern    SERVER_PORT;
		str_intern    REQUEST_URI;

		str_intern    HTTP_ACCEPT_LANGUAGE;
		str_intern    REQUEST_METHOD;
		str_intern    X_HTTP_METHOD_OVERRIDE;


		str_intern    _method;
		str_intern    php_input;
		str_intern    HTTPS;
		str_intern    https;
		str_intern    http;
		str_intern    off_key;
		str_intern    localhost;

		str_intern    HTTP_X_REQUESTED_WITH;
		str_intern    HTTP_USER_AGENT;
		str_intern    XMLHttpRequest;
		str_intern    HTTP_SOAPACTION;
		str_intern    soap_mime;

		str_intern    gethostbyname;
		str_intern    finfo_open;
		str_intern    finfo_file;
		str_intern    finfo_close;	
		str_intern    is_uploaded_file;
		str_intern    move_uploaded_file;
		


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