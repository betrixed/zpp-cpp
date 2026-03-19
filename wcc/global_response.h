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

		fn_call  	header_fn;


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

	typedef std::map<int, str_intern> StatusCodeMap;

	class Response_init : public state_init 
	{
	public:

		StatusCodeMap gStatusCodes;

		str_intern    headers_key;
		str_intern    file_key;
		str_intern    content_key;
		str_intern    cookies_key;
		str_intern    events_key;
		str_intern    sent_key;
		str_intern    Expires;
		str_intern    json_mime;
		str_intern    utf8;
		str_intern    Status;
		str_intern    readfile;
		str_intern    Refresh;
		str_intern    url_key;
		str_intern    Content_Type;
		str_intern    Location;
		str_intern    Content_Length;
		str_intern    HTTP_FS;
		str_intern    text_html;
		str_intern    eventqueue;
		str_intern    DIR_SEP;
		str_intern    AT_CHAR;
		str_intern    fire_key;
		str_intern    before_send;
		str_intern    after_send;
		str_intern    headers_sent;
		str_intern    headerfn_key;
		str_intern    Content_Description;
		str_intern    Content_Transfer_Encoding;
		str_intern    Content_Disposition;
		str_intern    binary_key;
		str_intern    file_transfer_key;
		str_intern    application_stream;

		void init() override;
		void end() override;

		str_rc getHttpCodeMsg(int code);
	};

	extern Response_init RSPD;

}; //namespace wcc
//global_response.h
#endif