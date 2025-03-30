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
		zobj_mgr server_;
		zobj_mgr get_;
		
		zobj_mgr files_;

		zobj_mgr request_;
		zobj_mgr post_;

		zstr_mgr host_;
		zstr_mgr body_;

		int  verb_;
		bool strictHost_;
		bool methodOverride_;
		bool spoof_;
		
		zval_mgr authCallback_;

	// hidden methods
		int fileCounter(htab_read data, bool onlySuccessful);
		zstr_mgr getBestQuality(htab_read parts, zstr_user name);

		htab_mgr getQualityHeader(zstr_user key, zstr_user name);
		htab_mgr resolveAuthorizationHeaders();

		htab_mgr smoothFiles(htab_read names, htab_read types,
			htab_read tmp_names, htab_read sizes,
			htab_read errors, zstr_user prefix);

		htab_read readServer();

	public:

		enum {
			PRE_AUTHORIZE, POST_AUTHORIZE
		};
		
		static base_obj_mgr<RequestGlobals> omg;

		virtual void debug_info(htab_write di);
		
		void construct();

		zobj_user getPost();
		zobj_user getQuery();
		zobj_user getServer();

		zobj_mgr makeFile(htab_read data, zstr_user name);

		void setAuthCallback(zval_user callback);

		htab_mgr getAcceptableContent();
		htab_mgr getBasicAuth();
		zstr_mgr getBestAccept();
		zstr_mgr getBestCharset();

		zstr_mgr getBestLanguage();
		zstr_mgr getBestQuality();

		zstr_mgr getClientAddress(bool trustHeader = false);

		htab_mgr getClientCharsets();

		zstr_mgr getContentType();

		htab_mgr getDigestAuth();

		zstr_mgr getHTTPReferer();

		zval_mgr getHeader(zstr_user header);

		void setHeader(zstr_user header, zval_user value);
		
		htab_mgr getHeaders();

		zstr_mgr getHttpHost();

		zval_mgr getJsonRawBody(bool asArray);

		htab_mgr getLanguages();

		int getMethod();

		bool getMethodOverride();

		void setMethodOverride(bool value);

		int getPort();


		
		zstr_mgr getRawBody();

		zstr_user getScheme();

		zstr_mgr getServerAddress();

		zstr_mgr getServerName();

		bool getSpoofMethod();

		void setSpoofMethod(bool val);

		zstr_mgr getURI(bool onlyPath = false);

		htab_mgr getUploadedFiles(bool onlySuccess = false, bool namekeys = false);

		zstr_mgr getUserAgent();

		bool hasFiles();

		bool hasHeader(zstr_user header);

		bool hasQuery(zstr_user key);

		bool hasRequest(zstr_user key);

		bool isAjax();

		bool isMethod(zval_user methods, bool strict = false);

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


	zstr_mgr gethostbyname(zstr_user str);
    zval_mgr finfo_open(int infoflags);
    zval_mgr finfo_file(zval_mgr& finfo, zstr_user path);
    zval_mgr finfo_close(zval_mgr& finfo);
    bool is_uploaded_file(zstr_user path);
    bool move_uploaded_file(zstr_user from, zstr_user to);
};

#endif