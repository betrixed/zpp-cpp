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

zend_class_entry* ce_IfEventQueue;
zend_class_entry* ce_IfFilter;
zend_class_entry* ce_IfEvent;
zend_class_entry* ce_IfCipher;


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

str_rc 
Response_init::getHttpCodeMsg(int code)
{
	//zend_printf("getHttpCodeMsg %ld\n", code);
	auto it = gStatusCodes.find(code);
	if (it != gStatusCodes.end())
	{
		return it->second;
	}
	//zend_printf("getHttpCodeMsg return null string\n");
	return str_rc();

};


Response_init RSPD;

base_obj_mgr<Response> Response::omg;

void
Response::construct(
			str_ptr  content,
			val_ptr   code,
			str_ptr  status)
{
	headers_ = Hmap::omg.new_zobj();
	hmap_ = zobj_toc<Hmap>(headers_);

	val_ptr(events_).setbool(false);
	sent_ = false;

	if (content.isNull())
	{
		content_ = str_empty();// zend empty string
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
Response::send_header(str_ptr header, bool replace,
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

htab_rw 
Response::writer()
{
	return hmap_->writer();
}

htab_ptr Response::reader() const
{
	return hmap_->reader();
}


void
Response::debug_info(htab_rw hw)
{
	// might as well reuse headers_key
	hw.set(RSPD.headers_key, headers_);

	hw.set(RSPD.file_key,  file_);

	hw.set(RSPD.content_key,  content_);

	hw.set(RSPD.cookies_key,  cookies_);

	hw.set(RSPD.events_key, events_);

	hw.set(RSPD.sent_key,  sent_);
}

obj_ptr 
Response::getHeaders()
{
	return headers_;
}

void Response::setHeaders(val_ptr headers)
{
	Hmap* hfrom = zobj_toc<Hmap> (headers.zobject());

	Hmap* hto = hdrs_obj();

	if (hto != hfrom) {
		htab_rw hw = hto->writer();
		htab_ptr data(hfrom->toArray());
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
Response::setExpires(val_ptr exptime)
{
	obj_ptr expires(exptime.zobject());

	datetime_obj utc(expires.clone());

	utc.setTimeZone(str_temp("UTC"));

	str_buf buf;

	buf << utc.format(str_temp("D, d M Y H:i:s")) << " GMT";

	str_rc time = buf.zstr();

	htab_rw hw = writer();
	hw.set(RSPD.Expires, time);
}

void Response::setNotModified()
{
	setStatusCode(304, str_temp("Not modified"));
}

void Response::setJsonContent(
	val_ptr content, 
	int jsonOptions)
{
	setContentType(RSPD.json_mime, RSPD.utf8);
	str_rc json = str_ptr::json_encode(content, jsonOptions);
	setContent(json);
	setStatusCode(200,str_empty());
}

val_rc 
Response::getStatusCode()
{
	Hmap* hdr = hdrs_obj();

	str_rc status = hdr->get(RSPD.Status);
	val_rc value;

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
			val_rc result = Response::readfile(file_);
			sent_ = !val_ptr(result).isFalse();
		}
	}
	return sent_;
}

 val_rc //static
 Response::readfile(str_ptr name)
 {
 	fn_call_args<1> call;

 	call.set_fname(RSPD.readfile);
 	zval* arg = call.argsptr();
 	ZVAL_STR(arg, name);
 	return call.call_fn();
 }

void 
Response::setHeader(str_ptr key, str_ptr value)
{
	htab_rw hw(writer());

	hw.set(key, value);
}

void 
Response::delay_redirect(str_ptr location, int delay)
{
	htab_rw hw(writer());

	str_buf buf;
	buf << delay;

	str_rc delaystr(buf.zstr());

	hw.set(RSPD.Refresh, delaystr);
	hw.set(RSPD.url_key, location);

	setStatusCode(303,str_empty());

	buf << "<!DOCTYPE html><html style><head>" 
		<< "<meta http-equiv=\"refresh\" content=\""
		<< delaystr << ";url=" << location << "\">"
		<< "</head><body><pre>Delayed REDIRECT to "
		<< location << " after " << delay 
		<< " seconds</pre></body></html>";

	str_rc out(buf.zstr());
	setContent(out);
}

void 
Response::setCookies(val_ptr bag)
{
	cookies_ = bag.zobject();
}

bool 
Response::sendCookies()
{
	if (!cookies_.isNull())
	{
		val_rc result = cookies_.call(str_temp("send"));
		return val_ptr(result).isTrue();
	}
	return true;
}


void 
Response::redirect(str_ptr location, bool external, int statusCode)
{
	if ((statusCode < 300) || (statusCode > 308))
	{
		statusCode = 302;
	}
	setStatusCode(302, str_empty());
	htab_rw hw(writer());

	hw.set(RSPD.Location, location);
}

void 
Response::setContentType(str_ptr ctype, str_ptr charset)
{
	setContentType(ctype.vstr(), charset.vstr());
}

void 
Response::resetHeaders()
{
	htab_rw hw(writer());
	hw.clear();
}

void Response::setContentLength(int clen)
{
	str_buf buf;

	buf << clen;
	str_rc pass(buf.zstr());
	writer().set(RSPD.Content_Length, pass);
}

void 
Response::setContentType(
	const std::string_view& ctype, 
	const std::string_view& charset)
{
	str_buf buf;
	buf << ctype;
	if (charset.size())
	{
		buf << "; charset=" << charset;
	}
	str_rc hvalue(buf.zstr());
	//showstr("hvalue", hvalue);

	htab_rw hw(writer());
	hw.set(RSPD.Content_Type, hvalue);
}

void 
Response::setContent(str_ptr  content)
{
	content_ = content;
}

void 
Response::setStatusCode(int icode, str_ptr  message)
{
	htab_ptr rawhdrs(reader());

	htab_walk wk;
	auto  key = wk.key();

	htab_rc   keylist;
	htab_rw rkeys(keylist);

	std::string_view needle = RSPD.HTTP_FS.vstr();

	for(wk.start(rawhdrs); wk.ok(); wk.next())
	{
		if (key.isString())
		{
			str_ptr hkey(key.zstr());

			int xpos = hkey.find(needle,0);

			if (xpos >= 0) {
				rkeys.push_back(hkey);
			}
		}
	}

	htab_rw hw = writer();
	if (rkeys.size()) 
	{
		hw.removal(rkeys);
	}

	str_rc msg;

	if ((icode != 0) && (message.size()==0))
	{

		msg = RSPD.getHttpCodeMsg(icode);

		if (msg.size()==0)
		{
			zend_throw_error(zend_ce_exception, "non-standard status code %d without message ", icode);
			//zend_printf("Return \n");
			return;
		}
	}
	else {
		msg = message;
	}
	str_buf buf;
	buf << icode << " " << msg;
	str_rc status(buf.zstr());

	str_buf hraw;
	hraw << "HTTP/1.1 " << status;


	str_rc rawstatus(hraw.zstr());

	val_rc null_value;
	hw.set(rawstatus, null_value);
	hw.set(RSPD.Status, status);
}

void 
Response::ajaxHtml(str_ptr  content)
{
	setContentType(RSPD.text_html, RSPD.utf8);
	setContent(content);
	setStatusCode(200,str_empty());
}

bool 
Response::hasContent()
{
	return (content_.size() > 0);
}

bool
Response::hasHeader(str_ptr name)
{
	htab_ptr rd(reader());
	return rd.has_key(name);
}

str_rc 
Response::getContent()
{
	return content_;
}

void 
Response::appendContent(str_ptr  content)
{
	str_buf buf;

	buf << content_ << content;

	content_ = buf.zstr();
}

obj_rc 
Response::getEventQueue()
{
	val_ptr test(events_);

	if (test.isFalse())
	{
		events_ = Services::service(RSPD.eventqueue);
	}
	// object or null
	return obj_rc(test.zobject());
}

void 
Response::ajaxJson(val_ptr  content)
{
	setJsonContent(content);
}

str_rc // protected
Response::attach_name(str_ptr uri, str_ptr suffix)
{
	val_ptr DIR_SEP = val_ptr::php_constant(RSPD.DIR_SEP); 
	str_ptr sDIR_SEP(DIR_SEP);
	str_rc t_uri(uri);

	t_uri.trim_self(sDIR_SEP.data(), str_rc::RTRIM);

	str_rc QREGEX = preg_quote(DIR_SEP, RSPD.AT_CHAR);

	str_buf buf;
	buf << "@[^" << QREGEX << "]+$@";

	str_rc regex(buf.zstr());

	preg filename_match(regex);

	str_rc filename;

	if (filename_match.matches(t_uri))
	{
		htab_ptr htab(filename_match.captures());
		filename = htab[int(0)];
	}

	if (suffix.size()) {
		QREGEX = preg_quote(suffix, RSPD.AT_CHAR);

		buf << "@" << QREGEX << "$@";

		regex = buf.zstr();
		val_rc rname = preg_replace(regex, str_rc::empty_str(), filename);

		filename = val_ptr(rname).zstr();

	}
	return filename;

}

val_rc 
Response::fireEvent(str_ptr eventType)
{
	obj_rc mgr = getEventQueue();
	val_rc result;

	if (mgr.ok())
	{
		fn_call_args<2> fire;
		fire.set_fci(mgr, RSPD.fire_key);

		zval* args = fire.argsptr();
		ZVAL_STR(args, eventType);
		ZVAL_OBJ(args+1, this->vobj());
		result = fire.call_fn();
	}
	else {
		// pretend it happened
		val_ptr(result).setbool(true); 
	}
	return result;
};

bool 
Response::sendHeaders()
{
	//zend_printf("Call getEventQueue\n");
	obj_rc mgr = getEventQueue();
	bool hasMgr =  mgr.ok();

	//showobj("event mgr", mgr);
	val_rc result;

	if (hasMgr)
	{
		result = fireEvent(RSPD.before_send);
		if (val_ptr(result).isFalse())
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

	htab_ptr rd(reader());

	std::string_view http_prefix = RSPD.HTTP_FS.vstr();

	str_rc hstr;
	str_buf buf;

	for(wk.start(rd); wk.ok(); wk.next())
	{
		str_rc harg = hkey.zstr();
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
	val_rc result = hsfn.call_fn();
	sent_ = val_ptr(result).isTrue();
	return sent_;
}

void Response::make_header(
	str_ptr name,
	str_ptr value
	)
{
	str_buf buf;

	buf << name << ": " << value;
	str_rc raw(buf.zstr());
	setRawHeader(raw);
}

void Response::setFileToSend(
	str_ptr path, 
	str_ptr attachName, 
	bool attachment)
{
	str_rc basePath;
	str_rc encoding;

	if (attachName.size())
	{
		basePath = attachName;
	}
	else {
		basePath = this->attach_name(path, str_empty());
	}

	if (attachment) {

		if (function_exists(STAB.mb_detect_order))
		{
			encoding = mb_detect_encoding(basePath, mb_detect_order(val_rc()), true);
		}
		val_rc null_value;

		make_header(RSPD.Content_Description, RSPD.file_transfer_key);
		make_header(RSPD.Content_Type, RSPD.application_stream);
		make_header(RSPD.Content_Transfer_Encoding, RSPD.binary_key);

		str_buf buf;
		str_rc temp;

		buf << "attachment; filename=";
		str_rc disposition(buf.zstr());

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
			basePath = addcslashes(basePath,str_temp("\15\17\\\""));

			const char dquote = '"';
			buf << disposition << dquote << basePath << dquote;
			temp = buf.zstr();

			make_header(RSPD.Content_Disposition, temp);
		}
	}
	file_ = path;
}

void 
Response::setRawHeader(str_ptr header)
{
	htab_rw hw(writer());
	val_rc null_value;

	hw.set(header,null_value);
}

}; // namespace wcc
 
using namespace wcc;
using namespace zpp;

//@@@@@@@@@@@@@@@@		 @@@@@@@@@@@@@@@@@@     @@@@@@@@@@@@@@@@@    @@@@@@@@@@@@@@@    @@@@@@@@@@@@@@@
ZEND_METHOD(Wcc_Response, __construct)
{
	zend_string* content = nullptr;
	zend_long code = 0;
	bool  null_code = true;
	zend_string* status = nullptr;

	ZEND_PARSE_PARAMETERS_START(0,3)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_NULL(content)
	Z_PARAM_LONG_OR_NULL(code, null_code)
	Z_PARAM_STR_OR_NULL(status)
	ZEND_PARSE_PARAMETERS_END();


	auto cobj = zval_toc<Response> (ZEND_THIS);

	val_rc arg_code;
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
	val_rc result = cobj->fireEvent(event);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Response, getEventQueue)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	obj_rc result = cobj->getEventQueue();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Response, getContent)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	str_rc result = cobj->getContent();
	result.move_zv(return_value);	
}

ZEND_METHOD(Wcc_Response, getHeaders)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	obj_ptr result = cobj->getHeaders();
	result.return_zv(return_value);	
}

ZEND_METHOD(Wcc_Response, getStatusCode)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Response> (ZEND_THIS);
	val_rc result = cobj->getStatusCode();
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
	ce_IfEvent = register_class_Wcc_IfEvent();
	ce_IfEventQueue = register_class_Wcc_IfEventQueue();
	ce_IfFilter = register_class_Wcc_IfFilter();
	ce_IfCipher = register_class_Wcc_IfCipher();

	auto ce = register_class_Wcc_Response();

	Response::omg.classEntry(ce);

	return SUCCESS;
}

//global_response.cpp
#endif