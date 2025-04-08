#ifndef GLOBAL_RESPONSE_H
#define GLOBAL_RESPONSE_H

/*
#ifndef HEADERS_RESPONSE_H
#include "headers_response.h"
#endif
*/



#include <map>

namespace wcc {

	using namespace zpp;

	class Response : public base_d {
	protected:
		zval_mgr events_;  // false|object|null
		zobj_mgr headers_; // Hmap object
		zstr_mgr file_;
		zstr_mgr content_;
		zobj_mgr cookies_; // some cookies interface
		bool     sent_;

		Hmap* hdrs_obj() const {
			return zobj_toc<Hmap>(headers_);
		}

		zstr_mgr attach_name(zstr_user uri, zstr_user suffix);

	public:
		static base_obj_mgr<Response> omg;

		static zval_mgr readfile(zstr_user name);

		void debug_info(htab_write hw) override;

		
		void construct(
			zstr_user  content,
			zval_user     code,
			zstr_user     status);

		void appendContent(zstr_user  content);
		bool hasContent();
		zstr_mgr getContent();

		void setCookies(zval_user bag);
		bool sendCookies();

		void delay_redirect(zstr_user location, int delay=2);
		void redirect(zstr_user location, bool external = false, int statusCode = 302);

		zobj_user getHeaders();
		void setHeaders(zval_user headers);

		void setNotModified();

		void setExpires(zval_user exptime);

		void setHeader(zstr_user key, zstr_user value);

		void setContentType(zstr_user ctype, zstr_user charset);
		void setContentType(const std::string_view& ctype, const std::string_view& charset);

		void setContent(zstr_user  content);
		void setStatusCode(int icode, zstr_user  message);

		void ajaxHtml(zstr_user  content);
		void ajaxJson(zval_user  content);

		void setJsonContent(zval_user content, int jsonOptions=0);

		

		bool send();
		bool sendHeaders();
		bool isSent();

		bool hasHeader(zstr_user name);

		zval_mgr getStatusCode();

		zobj_mgr getEventQueue();

		zval_mgr fireEvent(zstr_user eventType);

		void resetHeaders();
		void setContentLength(int clen);
		void setRawHeader(zstr_user header);
		
		void setFileToSend(zstr_user path, zstr_user attachName, bool attachement=true);
	};

	typedef std::map<int, zstr_intern> StatusCodeMap;

	class Response_init : public state_init 
	{
	public:

		StatusCodeMap gStatusCodes;

		zstr_intern    header_key;
		zstr_intern    file_key;
		zstr_intern    content_key;
		zstr_intern    cookies_key;
		zstr_intern    events_key;
		zstr_intern    sent_key;
		zstr_intern    Expires;
		zstr_intern    json_mime;
		zstr_intern    utf8;
		zstr_intern    Status;
		zstr_intern    readfile;
		zstr_intern    Refresh;
		zstr_intern    url_key;

		void init() override;
		void end() override;

		zstr_mgr getHttpCodeMsg(int code);
	};

	extern Response_init RSPD;

}; //namespace wcc
//global_response.h
#endif