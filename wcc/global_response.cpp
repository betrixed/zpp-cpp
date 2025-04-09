#ifndef GLOBAL_RESPONSE_CPP
#define GLOBAL_RESPONSE_CPP

#include <map>

#ifndef GLOBAL_RESPONSE_H
#include "global_response.h"
#endif

#ifndef WCC_SERVICES_H
#include "services.h"
#endif

#ifndef WCC_HMAP_H
#include "hmap.h"
#endif

#ifndef RESPONSE_ARGINFO_H
#define RESPONSE_ARGINFO_H
extern "C" {
	#include "stub/global_response_arginfo.h"	
};
#endif


namespace wcc {

using namespace zpp;

void Response_init::init()
{
	gStatusCodes = {
		{100, "Continue"},
		{101, "Switching Protocols"},
		{102, "Processing"},
		{103, "Early Hints"},

		{200, "OK"}, 
		{201, "Created"}, 
		{202, "Accepted"}, 
		{203, "Non-Authoritative Information"}, 
		{204, "No Content"}, 
		{205, "Reset Content"}, 
		{206, "Partial Content"}, 
		{207, "Multi-status"}, 
		{208, "Already Reported"}, 
		{226, "IM Used"}, 

		{300, "Multiple Choices"},
		{301, "Moved Permanently"},
		{302, "Found"},
		{303, "See Other"},
		{304, "Not Modified"},
		{305, "Use Proxy"},
		{306, "Switch Proxy"},
		{307, "Temporary Redirect"},
		{308, "Permanent Redirect"},

		{400, "Bad Request"},
		{401, "Unauthorized"},
		{402, "Payment Required"},
		{403, "Forbidden"},
		{404, "Not Found"},
		{405, "Method Not Allowed"},
		{406, "Not Acceptable"},
		{407, "Proxy Authentication Required"},
		{408, "Request Time-out"},
		{409, "Conflict"},
		{410, "Gone"},
		{411, "Length Required"},
		{412, "Precondition Failed"},
		{413, "Request Entity Too Large"},
		{414, "Request-URI Too Large"},
		{415, "Unsupported Media Type"},
		{416, "Requested range not satisfiable"},
		{417, "Expectation Failed"},
		{418, "I'm a teapot"},
		{421, "Misdirected Request"},
		{422, "Unprocessable Entity"},
		{423, "Locked"},
		{424, "Failed Dependency"},
		{425, "Unordered Collection"},
		{426, "Upgrade Required"},
		{428, "Precondition Required"},
		{429, "Too Many Requests"},
		{431, "Request Header Fields Too Large"},
		{451, "Unavailable For Legal Reasons"},
		{499, "Client Closed Request"},

		{500, "Internal Server Error"},
		{501, "Not Implemented"},
		{502, "Bad Gateway"},
		{503, "Service Unavailable"},
		{504, "Gateway Time-out"},
		{505, "HTTP Version not supported"},
		{506, "Variant Also Negotiates"},
		{507, "Insufficient Storage"},
		{508, "Loop Detected"},
		{510, "Not Extended"},
		{511, "Network Authentication Required"}
	};

	headers_key = "headers";
	file_key = "file";
	content_key = "content";
	cookies_key = "cookies";
	events_key = "events";
	sent_key = "sent";
	Expires =  "Expires";
	json_mime = "application/json";
	utf8 = "UTF-8";
	Status = "Status";
	readfile = "readfile";
	Refresh = "Refresh";
	url_key = "url";
	Content_Type = "Content-Type";
	Location = "Location";
	Content_Length = "Content-Length";
	HTTP_FS = "HTTP/";
	text_html = "text/html";
	eventqueue = "eventqueue";
	DIR_SEP = "DIRECTORY_SEPARATOR";
	AT_CHAR = "@";
	fire_key = "fire";
	before_send = "response:beforeSendHeaders";
	after_send = "response:afterSendHeaders";
	headers_sent = "headers_sent";
	headerfn_key = "header";
	Content_Description = "Content-Description";
	Content_Transfer_Encoding = "Content-Transfer-Encoding";
	Content_Disposition = "Content-Disposition";
	binary_key = "binary";
	file_transfer_key = "File Transfer";
	application_stream = "application/octet-stream";


}; // end initHttpCodes

void Response_init::end()
{
	gStatusCodes.clear();
}

zstr_mgr 
Response_init::getHttpCodeMsg(int code)
{
	//zend_printf("getHttpCodeMsg %ld\n", code);
	auto it = gStatusCodes.find(code);
	if (it != gStatusCodes.end())
	{
		return it->second;
	}
	//zend_printf("getHttpCodeMsg return null string\n");
	return zstr_mgr();

};


Response_init RSPD;

base_obj_mgr<Response> Response::omg;

void
Response::construct(
			zstr_user  content,
			zval_user   code,
			zstr_user  status)
{
	headers_ = Hmap::omg.new_zobj();
	hmap_ = zobj_toc<Hmap>(headers_);

	zval_user(events_).setbool(false);
	sent_ = false;

	if (content.isNull())
	{
		content_ = zstr_empty();// zend empty string
	}
	else {
		content_ = content;
	}
	if (code.isLong()) {
		zend_long icode = code.zlong();
		if (icode) {
			setStatusCode(code.zlong(), status);
		}
	}

	header_fn.set_fname(RSPD.headerfn_key);

}

void 
Response::send_header(zstr_user header, bool replace,
			int response_code)
{
	//showstr("send_header", header);
	if (sent_)
	{
		return;
	}
	zval* args = header_fn.argsptr();

	ZVAL_STR(args, header);
	ZVAL_BOOL(args+1, replace);
	ZVAL_LONG(args+2, response_code);

	// no result expected
	header_fn.call_fn();
}

htab_write 
Response::writer()
{
	return hmap_->writer();
}

htab_read Response::reader() const
{
	return hmap_->reader();
}


void
Response::debug_info(htab_write hw)
{
	// might as well reuse headers_key
	hw.set(RSPD.headers_key, headers_);

	hw.set(RSPD.file_key,  file_);

	hw.set(RSPD.content_key,  content_);

	hw.set(RSPD.cookies_key,  cookies_);

	hw.set(RSPD.events_key, events_);

	hw.set(RSPD.sent_key,  sent_);
}

zobj_user 
Response::getHeaders()
{
	return headers_;
}

void Response::setHeaders(zval_user headers)
{
	Hmap* hfrom = zobj_toc<Hmap> (headers.zobject());

	Hmap* hto = hdrs_obj();

	if (hto != hfrom) {
		htab_write hw = hto->writer();
		htab_read data(hfrom->toArray());
		htab_walk wk;
		auto  name = wk.key();
		auto  value = wk.value();

		for(wk.start(data); wk.ok(); wk.next())
		{
			hw.set(name.zstr(), value.zstr());
		}
	}
}

void 
Response::setExpires(zval_user exptime)
{
	zobj_user expires(exptime.zobject());

	datetime_obj utc(expires.clone());

	utc.setTimeZone(zstr_temp("UTC"));

	Hmap* hto = hdrs_obj();

	zstr_buffer buf;

	buf << utc.format(zstr_temp("D, d M Y H:i:s")) << " GMT";

	zstr_mgr time = buf.zstr();

	htab_write hw = writer();
	hw.set(RSPD.Expires, time);
}

void Response::setNotModified()
{
	setStatusCode(304, zstr_temp("Not modified"));
}

void Response::setJsonContent(
	zval_user content, 
	int jsonOptions)
{
	setContentType(RSPD.json_mime, RSPD.utf8);
	zstr_mgr json = zstr_user::json_encode(content, jsonOptions);
	setContent(json);
	setStatusCode(200,zstr_empty());
}

zval_mgr 
Response::getStatusCode()
{
	Hmap* hdr = hdrs_obj();

	zstr_mgr status = hdr->get(RSPD.Status);
	zval_mgr value;

	if (status.size()) 
	{
		value = status.substr(0,3);
		value.toLong();
	}

	return value;
}

bool 
Response::send()
{
	//zend_printf("Calling send()\n");
	if (sent_)
	{
		zend_throw_error(zend_ce_exception, "Response already sent");
	}
	sendCookies();

	if (!sendHeaders()) 
	{
		return false;
	}
	//zend_printf("sent headers\n");
	if (content_.size())
	{
		ZEND_WRITE(content_.data(), content_.size());
		sent_ = true;
	}
	else {
		if (!file_.isNull())
		{
			zval_mgr result = Response::readfile(file_);
			sent_ = !zval_user(result).isFalse();
		}
	}
	return sent_;
}

 zval_mgr //static
 Response::readfile(zstr_user name)
 {
 	fn_call_args<1> call;

 	call.set_fname(RSPD.readfile);
 	zval* arg = call.argsptr();
 	ZVAL_STR(arg, name);
 	return call.call_fn();
 }

void 
Response::setHeader(zstr_user key, zstr_user value)
{
	htab_write hw(writer());

	hw.set(key, value);
}

void 
Response::delay_redirect(zstr_user location, int delay)
{
	htab_write hw(writer());

	zstr_buffer buf;
	buf << delay;

	zstr_mgr delaystr(buf.zstr());

	hw.set(RSPD.Refresh, delaystr);
	hw.set(RSPD.url_key, location);

	setStatusCode(303,zstr_empty());

	buf << "<!DOCTYPE html><html style><head>" 
		<< "<meta http-equiv=\"refresh\" content=\""
		<< delaystr << ";url=" << location << "\">"
		<< "</head><body><pre>Delayed REDIRECT to "
		<< location << " after " << delay 
		<< " seconds</pre></body></html>";

	zstr_mgr out(buf.zstr());
	setContent(out);
}

void 
Response::setCookies(zval_user bag)
{
	cookies_ = bag.zobject();
}

bool 
Response::sendCookies()
{
	if (!cookies_.isNull())
	{
		zval_mgr result = cookies_.call(zstr_temp("send"));
		return zval_user(result).isTrue();
	}
	return true;
}


void 
Response::redirect(zstr_user location, bool external, int statusCode)
{
	if ((statusCode < 300) || (statusCode > 308))
	{
		statusCode = 302;
	}
	setStatusCode(302, zstr_empty());
	htab_write hw(writer());

	hw.set(RSPD.Location, location);
}

void 
Response::setContentType(zstr_user ctype, zstr_user charset)
{
	setContentType(ctype.vstr(), charset.vstr());
}

void 
Response::resetHeaders()
{
	htab_write hw(writer());
	hw.clear();
}

void Response::setContentLength(int clen)
{
	zstr_buffer buf;

	buf << clen;
	zstr_mgr pass(buf.zstr());
	writer().set(RSPD.Content_Length, pass);
}

void 
Response::setContentType(
	const std::string_view& ctype, 
	const std::string_view& charset)
{
	zstr_buffer buf;
	buf << ctype;
	if (charset.size())
	{
		buf << "; charset=" << charset;
	}
	zstr_mgr hvalue(buf.zstr());
	//showstr("hvalue", hvalue);

	htab_write hw(writer());
	hw.set(RSPD.Content_Type, hvalue);
}

void 
Response::setContent(zstr_user  content)
{
	content_ = content;
}

void 
Response::setStatusCode(int icode, zstr_user  message)
{
	htab_read rawhdrs(reader());

	htab_walk wk;
	auto  key = wk.key();
	auto  val = wk.value();

	htab_mgr   keylist;
	htab_write rkeys(keylist);

	std::string_view needle = RSPD.HTTP_FS.vstr();

	for(wk.start(rawhdrs); wk.ok(); wk.next())
	{
		if (key.isString())
		{
			zstr_user hkey(key.zstr());

			int xpos = hkey.find(needle,0);

			if (xpos >= 0) {
				rkeys.push_back(hkey);
			}
		}
	}

	htab_write hw = writer();
	if (rkeys.size()) 
	{
		hw.removal(rkeys);
	}

	zstr_mgr msg;

	if ((icode != 0) && (message.size()==0))
	{

		msg = RSPD.getHttpCodeMsg(icode);

		if (msg.size()==0)
		{
			zend_throw_error(zend_ce_exception, "non-standard status code %ld without message ", icode);
			//zend_printf("Return \n");
			return;
		}
	}
	else {
		msg = message;
	}
	zstr_buffer buf;
	buf << icode << " " << msg;
	zstr_mgr status(buf.zstr());

	zstr_buffer hraw;
	hraw << "HTTP/1.1 " << status;


	zstr_mgr rawstatus(hraw.zstr());

	zval_mgr null_value;
	hw.set(rawstatus, null_value);
	hw.set(RSPD.Status, status);
}

void 
Response::ajaxHtml(zstr_user  content)
{
	setContentType(RSPD.text_html, RSPD.utf8);
	setContent(content);
	setStatusCode(200,zstr_empty());
}

bool 
Response::hasContent()
{
	return (content_.size() > 0);
}

bool
Response::hasHeader(zstr_user name)
{
	htab_read rd(reader());
	return rd.has_key(name);
}

zstr_mgr 
Response::getContent()
{
	return content_;
}

void 
Response::appendContent(zstr_user  content)
{
	zstr_buffer buf;

	buf << content_ << content;

	content_ = buf.zstr();
}

zobj_mgr 
Response::getEventQueue()
{
	zval_user test(events_);

	if (test.isFalse())
	{
		events_ = Services::service(RSPD.eventqueue);
	}
	// object or null
	return zobj_mgr(test.zobject());
}

void 
Response::ajaxJson(zval_user  content)
{
	setJsonContent(content);
}

zstr_mgr // protected
Response::attach_name(zstr_user uri, zstr_user suffix)
{
	zval_user DIR_SEP = zval_user::php_constant(RSPD.DIR_SEP); 
	zstr_user sDIR_SEP(DIR_SEP);
	
	zstr_mgr t_uri = uri.trim(sDIR_SEP.data(), zstr_user::RTRIM);

	zstr_mgr QREGEX = preg_quote(DIR_SEP, RSPD.AT_CHAR);

	zstr_buffer buf;
	buf << "@[^" << QREGEX << "]+$@";

	zstr_mgr regex(buf.zstr());

	preg filename_match(regex);

	zstr_mgr filename;

	if (filename_match.matches(t_uri))
	{
		htab_read htab(filename_match.captures());
		filename = htab[int(0)];
	}

	if (suffix.size()) {
		QREGEX = preg_quote(suffix, RSPD.AT_CHAR);

		buf << "@" << QREGEX << "$@";

		regex = buf.zstr();
		zval_mgr rname = preg_replace(regex.data(), zstr_user::empty, filename);

		filename = zval_user(rname).zstr();

	}
	return filename;

}

zval_mgr 
Response::fireEvent(zstr_user eventType)
{
	zobj_mgr mgr = getEventQueue();
	zval_mgr result;

	if (mgr.ok())
	{
		fn_call_args<2> fire;
		fire.set_fci(mgr, RSPD.fire_key);

		zval* args = fire.argsptr();
		ZVAL_STR(args, eventType);
		ZVAL_OBJ(args+1, this->vobj());

		result = fire.call_fn();

		return result;
	}
	
	zval_user(result).setbool(true); // pretend
	return result;
};

bool 
Response::sendHeaders()
{
	//zend_printf("Call getEventQueue\n");
	zobj_mgr mgr = getEventQueue();
	bool hasMgr =  mgr.ok();

	//showobj("event mgr", mgr);
	zval_mgr result;

	if (hasMgr)
	{
		result = fireEvent(RSPD.before_send);
		if (zval_user(result).isFalse())
		{
			return false;
		}
	}

	bool ok = this->send_each();

	if (ok && hasMgr)
	{
		fireEvent(RSPD.after_send);
	}

	return ok;
}

bool 
Response::send_each()
{
	//zend_printf("headers_sent yet?\n");
	bool issent = headers_sent();

	if (issent)
	{
		//zend_printf("Already sent \n");
		return false;
	}
	//zend_printf("Not sent yet\n");

	htab_walk wk;
	auto hkey = wk.key();
	auto hvalue = wk.value();

	htab_read rd(reader());

	std::string_view http_prefix = RSPD.HTTP_FS.vstr();

	zstr_mgr hstr;
	zstr_buffer buf;

	for(wk.start(rd); wk.ok(); wk.next())
	{
		zstr_mgr harg = hkey.zstr();
		if (!hvalue.isNull())
		{
			

			buf << harg << ": " << hvalue.zstr();
			hstr = buf.zstr();
			//showstr("hstr 1", hstr);
			send_header(hstr, true);

		}
		else {
			if ( (harg.find(':') >= 0) || (harg.subview(0,5)==http_prefix))
			{
				send_header(harg,true);
			}
			else 
			{
				buf << harg << ": ";
				hstr = buf.zstr();
				//showstr("hstr 2", hstr);
				send_header(hstr,true);
			}
		}
	}
	return true;
}

bool 
Response::isSent()
{
	return sent_;
}

bool 
Response::headers_sent()
{
	if (sent_)
	{
		return sent_;
	}
	fn_call hsfn;

	hsfn.set_fname(RSPD.headers_sent);
	zval_mgr result = hsfn.call_fn();
	sent_ = zval_user(result).isTrue();
	return sent_;
}

void Response::make_header(
	zstr_user name,
	zstr_user value
	)
{
	zstr_buffer buf;

	buf << name << ": " << value;
	zstr_mgr raw(buf.zstr());
	setRawHeader(raw);
}

void Response::setFileToSend(
	zstr_user path, 
	zstr_user attachName, 
	bool attachment)
{
	zstr_mgr basePath;
	zstr_mgr encoding;

	if (attachName.size())
	{
		basePath = attachName;
	}
	else {
		basePath = this->attach_name(path, zstr_empty());
	}

	if (attachment) {

		if (function_exists(STAB.mb_detect_order))
		{
			encoding = mb_detect_encoding(basePath, mb_detect_order(zval_mgr()), true);
		}
		zval_mgr null_value;

		make_header(RSPD.Content_Description, RSPD.file_transfer_key);
		make_header(RSPD.Content_Type, RSPD.application_stream);
		make_header(RSPD.Content_Transfer_Encoding, RSPD.binary_key);

		zstr_buffer buf;
		zstr_mgr temp;

		buf << "attachment; filename=";
		zstr_mgr disposition(buf.zstr());

		if(encoding.vstr() != "ASCII") 
		{
			basePath = rawurlencode(basePath);
			temp = encoding.to_lower();

			buf << disposition << basePath
			    << "; filename*=" 
			    << temp << "''" << basePath;
			temp =  buf.zstr();

			make_header(RSPD.Content_Disposition, temp);
		}
		else {
			basePath = addcslashes(basePath,zstr_temp("\15\17\\\""));

			const char dquote = '"';
			buf << disposition << dquote << basePath << dquote;
			temp = buf.zstr();

			make_header(RSPD.Content_Disposition, temp);
		}
	}
	file_ = path;
}

void 
Response::setRawHeader(zstr_user header)
{
	htab_write hw(writer());
	zval_mgr null_value;

	hw.set(header,null_value);
}

}; // namespace wcc
 
//@@@@@@@@@@@@@@@@		 @@@@@@@@@@@@@@@@@@     @@@@@@@@@@@@@@@@@    @@@@@@@@@@@@@@@    @@@@@@@@@@@@@@@
ZEND_METHOD(Wcc_Response, __construct)
{
	zend_string* content = nullptr;
	zend_long code = 0;
	bool  null_code;
	zend_string* status = nullptr;

	ZEND_PARSE_PARAMETERS_START(0,3)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_NULL(content)
	Z_PARAM_LONG_OR_NULL(code,null_code)
	Z_PARAM_STR_OR_NULL(status)
	ZEND_PARSE_PARAMETERS_END();


	auto cobj = zval_toc<Response> (ZEND_THIS);

	zval_mgr arg_code;
	if (!null_code)
	{
		arg_code = code;
	}

	cobj->construct(content,arg_code,status);
}

ZEND_METHOD(Wcc_Response, ajaxHtml)
{
	zend_string* content;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(content)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	cobj->ajaxHtml(content);
}

ZEND_METHOD(Wcc_Response, ajaxJson)
{
	zval* data;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	cobj->ajaxJson(data);
}


ZEND_METHOD(Wcc_Response, appendContent)
{
	zend_string* content;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(content)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	cobj->appendContent(content);	
}

ZEND_METHOD(Wcc_Response, delay_redirect)
{
	zend_string* location;
	zend_long     delay;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(location)
	Z_PARAM_LONG(delay)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	cobj->delay_redirect(location, delay);
}

ZEND_METHOD(Wcc_Response, fireEvent)
{
	zend_string* event;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(event)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	zval_mgr result = cobj->fireEvent(event);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Response, getEventQueue)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	zobj_mgr result = cobj->getEventQueue();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Response, getContent)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	zstr_mgr result = cobj->getContent();
	result.move_zv(return_value);	
}

ZEND_METHOD(Wcc_Response, getHeaders)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	zobj_user result = cobj->getHeaders();
	result.return_zv(return_value);	
}

ZEND_METHOD(Wcc_Response, getStatusCode)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	zval_mgr result = cobj->getStatusCode();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Response, hasContent)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	bool result = cobj->hasContent();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_Response, hasHeader)
{
	zend_string* name;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	bool result = cobj->hasHeader(name);
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_Response, isSent)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	bool result = cobj->isSent();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_Response, redirect)
{
	zend_string* location;
	bool         external = false;
	zend_long    code = 302;

	ZEND_PARSE_PARAMETERS_START(1,3)
	Z_PARAM_STR(location)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(external)
	Z_PARAM_LONG(code)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	cobj->redirect(location, external, code);

}

ZEND_METHOD(Wcc_Response, resetHeaders)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	cobj->resetHeaders();
}

ZEND_METHOD(Wcc_Response, send)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	bool result = cobj->send();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_Response, sendCookies)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	bool result = cobj->sendCookies();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_Response, sendHeaders)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	bool result = cobj->sendHeaders();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_Response, setContent)
{
	zend_string* content;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(content)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	cobj->setContent(content);
}

ZEND_METHOD(Wcc_Response, setContentLength)
{
	zend_long  length;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(length)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	cobj->setContentLength(length);	
}

ZEND_METHOD(Wcc_Response, setContentType)
{
	zend_string* contentType;
	zend_string* charset;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_STR(contentType)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_NULL(charset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	cobj->setContentType(contentType, charset);		
}

ZEND_METHOD(Wcc_Response, setCookies)
{
	zval*  bag;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT(bag)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	cobj->setCookies(bag);
}

ZEND_METHOD(Wcc_Response, setExpires)
{
	zval*  expiry;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT(expiry)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	cobj->setExpires(expiry);
}

ZEND_METHOD(Wcc_Response, setFileToSend)
{
	zend_string* filePath;
	zend_string* attachName = nullptr;
	bool    	 attach = true;

	ZEND_PARSE_PARAMETERS_START(1,3)
	Z_PARAM_STR(filePath)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_NULL(attachName)
	Z_PARAM_BOOL(attach)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	cobj->setFileToSend(filePath, attachName, attach);	

}

ZEND_METHOD(Wcc_Response, setHeader)
{
	zend_string* name;
	zend_string* value;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(name)
	Z_PARAM_STR(value)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	cobj->setHeader(name, value);	
}

ZEND_METHOD(Wcc_Response, setHeaders)
{
	zval*  hdrs;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT(hdrs)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	cobj->setHeaders(hdrs);	
}

ZEND_METHOD(Wcc_Response, setJsonContent)
{
	zval* data;
	zend_long  options = 0;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_ARRAY_OR_OBJECT(data)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(options)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	cobj->setJsonContent(data,options);
}

ZEND_METHOD(Wcc_Response, setNotModified)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	cobj->setNotModified();
}

ZEND_METHOD(Wcc_Response, setRawHeader)
{
	zend_string* value;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(value)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	cobj->setRawHeader(value);
}

ZEND_METHOD(Wcc_Response, setStatusCode)
{
	zend_long code;
	zend_string* msg = nullptr;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_LONG(code)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR(msg)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	cobj->setStatusCode(code, msg);	
}

PHP_MINIT_FUNCTION(Wcc_Response_reg)
{

	auto ce = register_class_Wcc_Response();

	Response::omg.classEntry(ce);

	return SUCCESS;
}

//global_response.cpp
#endif