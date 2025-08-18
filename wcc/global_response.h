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

	class Hmap;

	class Response : public base_d {
	protected:
		val_rc events_;  // false|object|null
		obj_rc headers_; // Hmap object
		Hmap*    hmap_;    // Hmap cobj for convenience

		str_rc file_;
		str_rc content_;
		obj_rc cookies_; // some cookies interface
		bool     sent_;

		Hmap* hdrs_obj() const {
			return hmap_;
		}

		fn_call_args<3>  header_fn;


		str_rc attach_name(str_ptr uri, str_ptr suffix);
		bool send_each();
		void make_header(str_ptr name, str_ptr value);

	public:
		static base_obj_mgr<Response> omg;

		static val_rc readfile(str_ptr name);

		virtual void debug_info(htab_rw hw);

		
		void construct(
			str_ptr  content,
			val_ptr     code,
			str_ptr     status);

		void appendContent(str_ptr  content);
		bool hasContent();
		str_rc getContent();

		void setCookies(val_ptr bag);
		bool sendCookies();

		void delay_redirect(str_ptr location, int delay=2);
		void redirect(str_ptr location, bool external = false, int statusCode = 302);

		obj_ptr getHeaders();
		void setHeaders(val_ptr headers);

		void setNotModified();

		void setExpires(val_ptr exptime);

		void setHeader(str_ptr key, str_ptr value);

		void setContentType(str_ptr ctype, str_ptr charset);
		void setContentType(const std::string_view& ctype, const std::string_view& charset);

		void setContent(str_ptr  content);
		void setStatusCode(int icode, str_ptr  message);

		void ajaxHtml(str_ptr  content);
		void ajaxJson(val_ptr  content);

		void setJsonContent(val_ptr content, int jsonOptions=0);

		htab_rw writer();

        htab_ptr reader() const;

		bool send();
		bool sendHeaders();
		bool isSent();

		void send_header(str_ptr header, bool replace = true,
			int response_code = 0);

		bool headers_sent();

		bool hasHeader(str_ptr name);

		val_rc getStatusCode();

		obj_rc getEventQueue();

		val_rc fireEvent(str_ptr eventType);

		void resetHeaders();
		void setContentLength(int clen);
		void setRawHeader(str_ptr header);
		
		void setFileToSend(str_ptr path, str_ptr attachName, bool attachement=true);
	};

	typedef std::map<int, zstr_intern> StatusCodeMap;

	class Response_init : public state_init 
	{
	public:

		StatusCodeMap gStatusCodes;

		zstr_intern    headers_key;
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
		zstr_intern    Content_Type;
		zstr_intern    Location;
		zstr_intern    Content_Length;
		zstr_intern    HTTP_FS;
		zstr_intern    text_html;
		zstr_intern    eventqueue;
		zstr_intern    DIR_SEP;
		zstr_intern    AT_CHAR;
		zstr_intern    fire_key;
		zstr_intern    before_send;
		zstr_intern    after_send;
		zstr_intern    headers_sent;
		zstr_intern    headerfn_key;
		zstr_intern    Content_Description;
		zstr_intern    Content_Transfer_Encoding;
		zstr_intern    Content_Disposition;
		zstr_intern    binary_key;
		zstr_intern    file_transfer_key;
		zstr_intern    application_stream;

		void init() override;
		void end() override;

		str_rc getHttpCodeMsg(int code);
	};

	extern Response_init RSPD;

}; //namespace wcc
//global_response.h
#endif