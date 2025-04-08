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

	header_key = "headers";
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
}


void
Response::debug_info(htab_write hw)
{
	// might as well reuse header_key
	hw.set(RSPD.header_key, headers_);

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

	zstr_mgr time(std::move(buf));

	htab_write hw = hto->writer();
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
	Hmap* hdr = hdrs_obj();
	htab_write hw(hdr->writer());

	hw.set(key, value);
}

void 
Response::delay_redirect(zstr_user location, int delay)
{
	Hmap* hdr = hdrs_obj();
	htab_write hw(hdr->writer());

	zstr_buffer buf;
	buf << delay;
	zstr_mgr delaystr(std::move(buf));

	hw.set(RSPD.Refresh, delaystr);
	hw.set(RSPD.url_key, location);

	setStatusCode(303,zstr_empty());

	buf << "<!DOCTYPE html><html style><head>" 
		<< "<meta http-equiv=\"refresh\" content=\""
		<< delaystr << ";url=" << location << "\">"
		<< "</head><body><pre>Delayed REDIRECT to "
		<< location << " after " << delay 
		<< " seconds</pre></body></html>";

	zstr_mgr out(std::move(buf));
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
		return zstr_user(result).isTrue();
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
	setStatusCode(302, zstr_base());

	hdrs_obj()->set(zstr_temp("Location"), location);
}

void 
Response::setContentType(zstr_user ctype, zstr_user charset)
{
	setContentType(ctype.vstr(), charset.vstr());
}

void 
Response::resetHeaders()
{
	hdrs_obj()->reset();
}

void Response::setContentLength(int clen)
{
	zval_mgr temp(clen);
	zstr_pass pass(temp.to_zstr());
	hdrs_obj()->set(zstr_temp("Content-Length"), pass);
}

void 
Response::setContentType(const std::string_view& ctype, const std::string_view& charset)
{
	zstr_buffer buf;
	buf << ctype;
	if (charset.size())
	{
		buf << "; charset=" << charset;
	}
	zstr_mgr hvalue(std::move(buf));

	hdrs_obj()->set(wis->content_type, hvalue);
}

void 
Response::setContent(zstr_user  content)
{
	content_ = content;
}

void 
Response::setStatusCode(int icode, zstr_user  message)
{

	Headers* hobj = hdrs_obj();

	htab_ptr rawhdrs = hobj->toArray();

	htab_walk wk;
	auto& key = wk.key();
	auto& val = wk.value();

	

	for(wk.start(rawhdrs); wk.ok(); wk.next())
	{
		if (key.isString())
		{
			zstr_user hkey(key.zstr());
			int xpos = hkey.find("HTTP/");
			if (xpos >= 0) {
				hobj->remove(hkey);
			}
		}
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

	zstr_mgr status(std::move(buf));


	zstr_buffer hraw;
	hraw << "HTTP/1.1 " << status;


	zstr_mgr rawstatus(std::move(hraw));


	hobj->setRaw(rawstatus);

	hobj->set(zstr_temp("Status"), status);


}

void 
Response::ajaxHtml(zstr_user  content)
{
	setContentType(wis->text_html, wis->utf8);
	setContent(content);
	setStatusCode(200,wis->emptystr);
}

bool 
Response::hasContent()
{
	return (content_.size() > 0);
}

bool
Response::hasHeader(zstr_user name)
{
	Headers* hobj = hdrs_obj();
	return hobj->has(name);
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

	content_ = std::move(buf.zstr());
}

zobj_mgr 
Response::getEventQueue()
{
	if (events_.isFalse())
	{
		//zend_printf("get eventqueue\n");
		events_ = Wcc_Services::service(zstr_temp("eventqueue"));
		//showmem("Return Event Queue", events_);
	}
	return std::move(events_);
}

void 
Response::ajaxJson(zval_user  content)
{
	setJsonContent(content);
}

zstr_mgr // protected
Response::attach_name(zstr_user uri, zstr_user suffix)
{
	zval_user DIR_SEP = zstr_base::zend_constant(wis->DIR_SEP); 
	zstr_user sDIR_SEP(DIR_SEP);
	
	zstr_user s_uri(uri);

	zstr_mgr t_uri = s_uri.trim(sDIR_SEP.data(), zstr_user::RTRIM);
	zstr_temp delimiter("@");

	zstr_mgr QREGEX = preg_quote(DIR_SEP, delimiter);

	zstr_buffer buf;
	buf << "@[^" << QREGEX << "]+$@";

	preg filename_match(buf.zstr());

	zstr_mgr filename;

	if (filename_match.matches(t_uri))
	{
		htab_ptr htab = filename_match.array();
		filename = htab[int(0)];
	}
	else {
		filename.clear();
	}

	if (suffix.size()) {
		QREGEX = preg_quote(suffix, delimiter);

		buf << "@" << QREGEX << "$@";

		zstr_mgr regex(std::move(buf));
		zval_mgr rname = preg_replace(regex.data(), "", filename);

		filename = rname.zstr();

	}
	return filename;

}

zval_mgr 
Response::fireEvent(zstr_user eventType)
{
	zobj_mgr mgr = getEventQueue();
	zval_mgr result;

	if (!mgr.isNull())
	{
		zval_mgr arg1(eventType);
		zval_mgr arg2(this->zobj());
		result = mgr.call(zstr_temp("fire"),arg1, arg2);
		return result;
	}
	
	result.setbool(true); // pretend
	return result;
};

bool 
Response::sendHeaders()
{
	//zend_printf("Call getEventQueue\n");
	zobj_mgr mgr = getEventQueue();

	//showobj("event mgr", mgr);
	zval_mgr result;

	bool hasMgr = !mgr.isNull();

	if (hasMgr)
	{
		result = fireEvent(zstr_temp("response:beforeSendHeaders"));
		if (result.isFalse())
		{
			return false;
		}
	}
	//zend_printf("Call headers->send\n");

	bool ok = hdrs_obj()->send();

	if (ok && hasMgr)
	{
		fireEvent(zstr_temp("response:afterSendHeaders"));
	}

	return ok;
}

bool 
Response::isSent()
{
	return sent_;
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
		basePath = this->attach_name(path, zstr_base());
	}

	Headers* hobj = hdrs_obj();

	if (attachment) {

		if (function_exists(wis->mb_detect_order))
		{
			encoding = mb_detect_encoding(basePath, mb_detect_order(zval_mgr()), true);
		}
		hobj->setRaw(zstr_temp("Content-Description: File Transfer"));
		hobj->setRaw(zstr_temp("Content-Type: application/octet-stream"));
		hobj->setRaw(zstr_temp("Content-Transfer-Encoding: binary"));

		zstr_temp disposition("Content-Disposition: attachment; filename=");

		if(encoding.vstr() != "ASCII") 
		{
			basePath = rawurlencode(basePath);
			zstr_buffer buf;

			buf << disposition << basePath
			    << "; filename*=" << encoding.to_lower() << "''" << basePath;

			hobj->setRaw(buf.zstr());
		}
		else {
			basePath = addcslashes(basePath,zstr_temp("\15\17\\\""));

			const char dquote = '"';
			zstr_buffer buf;
			buf << disposition << dquote << basePath << dquote;
			
			hobj->setRaw(buf.zstr());
		}
	}
	file_ = path;
}

void 
Response::setRawHeader(zstr_user header)
{
	Headers* hobj = hdrs_obj();
	hobj->setRaw(header);
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

	cobj->construct(content,arg_code.ptr(),status);
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
	zval_mgr result = cobj->getEventQueue();
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
	zval_mgr result = cobj->getHeaders();
	result.move_zv(return_value);	
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