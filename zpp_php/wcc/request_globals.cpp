#ifndef REQUEST_GLOBALS_CPP
#define REQUEST_GLOBALS_CPP

#ifndef REQUEST_GLOBALS_H
#include "request_globals.h"
#endif

#ifndef WCC_ROUTE_H
#include "route.h"
#endif

#ifndef FILE_UPLOAD_H
#include "file_upload.h"
#endif

//#define DBG_REQUEST_GLOBALS

#ifdef DBG_REQUEST_GLOBALS
#ifndef WCC_DEBUGLOG_H
#include "debuglog.h"
#endif
#endif

#ifndef REQUEST_GLOBALS_ARGINFO
#define REQUEST_GLOBALS_ARGINFO
extern "C" {
	#include "stub/request_globals_arginfo.h"
};
#endif

extern "C" {
	#include <ext/standard/base64.h>	
};

namespace wcc {
using namespace zpp;

	base_obj_mgr<RequestGlobals> RequestGlobals::omg;

	request_init RQit;

void
request_init::init() {
	quality = "quality";
	PHP_AUTH_USER = "PHP_AUTH_USER";
	Php_Auth_User = "Php-Auth-User";

	PHP_AUTH_PW = "PHP_AUTH_PW";
	Php_Auth_Pw = "Php-Auth-Pw";

	PHP_AUTH_DIGEST = "PHP_AUTH_DIGEST";
	Php_Auth_Digest = "Php-Auth-Digest";

	HTTP_AUTHORIZATION = "HTTP_AUTHORIZATION";
	REDIRECT_HTTP_AUTHORIZATION = "REDIRECT_HTTP_AUTHORIZATION";

	Authorization = "Authorization";

	language = "language";
	username = "username";
	accept = "accept";
	size_key = "size";
	error_key = "error";
	tmp_name = "tmp_name";
	password = "password";
	charset = "charset";
	namekey = "name";
	typekey = "type";
	key_key = "key";
	ext_key = "ext";
	realtype_key = "realType"; 
	equals_key = "="; 


	basic_sp = "basic ";
	bearer_sp = "bearer ";
	digest_sp = "digest ";
	step_key = "step";
	server_key = "server";
	colon_key = ":";
	headers_key = "headers";

	get_key = "get";
	post_key = "post";
	request_key = "request";
	files_key = "files";
	verb_key = "verb";
	strict_host = "strict_host";
	spoof_key = "spoof";
	method_override = "method_override";

	G_SERVER = "_SERVER";
	G_GET = "_GET";
	G_REQUEST = "_REQUEST";
	G_POST = "_POST";
	G_FILES = "_FILES";
	G_SESSION = "_SESSION";
	G_GLOBALS = "_GLOBALS";
	
	HTTP_ACCEPT = "HTTP_ACCEPT";
	HTTP_X_FORWARDED_FOR = "HTTP_X_FORWARDED_FOR";
	HTTP_CLIENT_IP = "HTTP_CLIENT_IP";
	REMOTE_ADDR = "REMOTE_ADDR";
	comma_key = ",";
	HTTP_ACCEPT_CHARSET = "HTTP_ACCEPT_CHARSET";
	CONTENT_TYPE = "CONTENT_TYPE";
	CONTENT_LENGTH = "CONTENT_LENGTH";
	CONTENT_MD5 = "CONTENT_MD5";

	HTTP_REFERER = "HTTP_REFERER";
	hyphen_key = "-";
	underscore = "_";
	blank_key = " ";
	HTTP_ = "HTTP_";
	HTTP_HOST = "HTTP_HOST";
	SERVER_NAME = "SERVER_NAME";
	SERVER_ADDR = "SERVER_ADDR";
	SERVER_PORT = "SERVER_PORT";
	REQUEST_URI = "REQUEST_URI";

	HTTP_ACCEPT_LANGUAGE = "HTTP_ACCEPT_LANGUAGE";
	REQUEST_METHOD = "REQUEST_METHOD";
	X_HTTP_METHOD_OVERRIDE = "X_HTTP_METHOD_OVERRIDE";
	_method = "_method";
	php_input = "php://input";
	HTTPS = "HTTPS";
	https = "https";
	http = "http";
	off_key = "off";
	localhost = "localhost";

	HTTP_X_REQUESTED_WITH = "HTTP_X_REQUESTED_WITH";
	HTTP_USER_AGENT = "HTTP_USER_AGENT";
	XMLHttpRequest = "XMLHttpRequest";
	HTTP_SOAPACTION = "HTTP_SOAPACTION"; // who still uses this?
	soap_mime = "application/soap+xml";

	gethostbyname = "gethostbyname";
	finfo_open = "finfo_open";
	finfo_file = "finfo_file";
	finfo_close = "finfo_close";	

    is_uploaded_file = "is_uploaded_file";
    move_uploaded_file = "move_uploaded_file";
	
}


str_rc 
gethostbyname(str_ptr s)
{
	fn_call call(RQit.gethostbyname);

    fn_params<1>  fn(call);
    val_ptr::string_bind(fn.argsptr(), s);
    return fn.str();

}

val_rc 
finfo_open(int infoflags)
{
	fn_call call(RQit.finfo_open);
    fn_params<1>  fn(call);
    ZVAL_LONG(fn.argsptr(), infoflags);
    return fn.mixed();
}

val_rc 
finfo_file(val_rc& finfo, str_ptr path)
{
	fn_call call(RQit.finfo_file);
    fn_params<2>  fn(call);
    zval* pargs = fn.argsptr();

    ZVAL_COPY_VALUE(pargs, finfo);
    val_ptr::string_bind(pargs+1, path);
    return fn.mixed();
}

val_rc 
finfo_close(val_rc& finfo)
{
	fn_call call(RQit.finfo_close);
    fn_params<1>  fn(call);
    ZVAL_COPY_VALUE(fn.argsptr(), finfo);
    return fn.mixed();
}

bool 
is_uploaded_file(str_ptr path)
{
	fn_call call(RQit.is_uploaded_file);
    fn_params<1>  fn(call);
    val_ptr::string_bind(fn.argsptr(), path);
    return fn.zbool();
}

bool
move_uploaded_file(str_ptr from, str_ptr to)
{
	fn_call call(RQit.move_uploaded_file);
	fn_params<2>  fn(call);
	zval* pargs = fn.argsptr();
	val_ptr::string_bind(pargs, from);
	val_ptr::string_bind(pargs+1, to);
	return fn.zbool();
}

//protected
htab_ptr RequestGlobals::readServer()
{
	return Hmap::map_htab(server_);
}

//protected
int 
RequestGlobals::fileCounter(htab_ptr data, bool onlySuccessful)
{
	int result = 0;

	for_key_value wk;

	for(wk.start(data); wk.ok(); wk.next())
	{
		val_ptr value(wk.value());
		if (value.isArray())
		{
			htab_ptr fsub(value.zarray());

			result += fileCounter(fsub,onlySuccessful);
			continue; 
		}
		if (!value.isNull() || !onlySuccessful)
		{
			result++;
		}
	}
	return result;
}

obj_rc
RequestGlobals::makeFile(htab_ptr fdata, str_ptr name)
{
	obj_rc result = FileUpload::omg.new_zobj();
	FileUpload* fup = zobj_toc<FileUpload>(result);
	fup->construct(fdata, name);

	return result;
}

str_rc 
RequestGlobals::getBestQuality(htab_ptr parts, str_ptr name)
{
	int i = 0;
	double quality = 0.0;

	str_rc  result;

	for_key_value wk;
	

	for(wk.start(parts); wk.ok(); wk.next(), i++)
	{
		val_ptr value(wk.value());

		if (!value.isArray()) continue;

		htab_ptr accept(value.zarray());

		val_ptr dval( accept.get(RQit.quality) );
		val_ptr sval( accept.get(name) );

		if (i == 0)
		{
			quality = dval.zdouble();
			result = sval.zstr();
		}
		else 
		{
			double temp = dval.zdouble();
			if (temp > quality)
			{
				quality = temp;
				result = sval.zstr();
			}
		}
	}
	return result;
}

htab_rc 
RequestGlobals::getQualityHeader(str_ptr key, str_ptr name)
{
	htab_rc result;
	htab_rw qh(result);

	htab_ptr server(readServer());

	str_rc data = server.get(key);

	if (data.isNull())
	{
		return result;
	}

	val_rc parts_split = preg_split("/,\\s*/", data, -1, preg::SPLIT_NO_EMPTY);
	val_ptr parts(parts_split);

	if (!parts.isArray())
	{
		return result;
	}

	preg 	data_split("/\\s*;\\s*/", preg::SPLIT_NO_EMPTY);

	for_key_value wk;

	for(wk.start(parts.zarray()); wk.ok(); wk.next())
	{
		val_ptr part(wk.value());

		htab_rc headerParts;
		htab_rw collect(headerParts);

		val_rc rs = data_split.splits(part.zstr(), -1);
		val_ptr sc_split(rs);

		if (!sc_split.isArray()) continue;

		for_key_value w2;

		
		for(w2.start(sc_split.zarray()); w2.ok(); w2.next())
		{
			val_ptr headerPartVal(w2.value());

			str_rc  headerPart = headerPartVal.zstr();

			if (headerPart.find('=') >= 0) 
			{
				htab_rc tuple = explode(RQit.equals_key, headerPart, 2);
			}
			else {
				collect.set(name, headerPartVal);
				collect.set(RQit.quality, 1.0);
			}
		}
		qh.push_back(collect);
	}
	return result;
}

htab_rc 
RequestGlobals::resolveAuthorizationHeaders()
{
	val_ptr auth_call(authCallback_);

	bool doCallback = auth_call.ok();
	htab_rc result;
	htab_rw headers(result);

	if (doCallback) {
		htab_rc arg_mgr;	
		htab_rw args(arg_mgr);

		args.set(RQit.step_key, PRE_AUTHORIZE);
		args.set(RQit.server_key, server_);

		val_rc arg1(arg_mgr);

		val_rc ret_headers;

		if (callable_fn( ret_headers, authCallback_, 1, arg1 ))
		{
			val_ptr auth(ret_headers);
			if (auth.isArray())
			{
				headers.merge(auth.zarray());
			}
		}
	}

	htab_ptr server(readServer());
	val_ptr user = server.get(RQit.PHP_AUTH_USER);
	val_ptr apw =  server.get(RQit.PHP_AUTH_PW);

/*
	//zstr_make<false> userkey("Php-Auth-User");
	//zstr_make<false> pwkey("Php-Auth-Pw");
	zstr_make<false> authorization("Authorization");
	zstr_make<false> digestkey("Php-Auth-Digest");
*/

	if (user.isString() && apw.isString()) 
	{
		headers.set(RQit.Php_Auth_User, user);
		headers.set(RQit.Php_Auth_Pw, apw);
	}
	else {

		str_rc auth_hdr = server.get(RQit.HTTP_AUTHORIZATION);
		if (auth_hdr.isNull())
		{
			auth_hdr = server.get(RQit.REDIRECT_HTTP_AUTHORIZATION);
		}
		if ( auth_hdr.size() ) 
		{
			str_rc lc_auth_hdr(auth_hdr);
			lc_auth_hdr.lowercase();

			str_rc test(lc_auth_hdr);

			if (test.starts_with(RQit.basic_sp)) 
			{
				str_rc decode = str_rc::base64_decode((const unsigned char*)auth_hdr.data() + 6, 
																auth_hdr.size()-6);

				val_rc  excode = explode(RQit.colon_key, decode, 2);
				val_ptr parts(excode);

				if (parts.isArray())
				{
					htab_ptr parts2(parts.zarray());
					headers.set(RQit.Php_Auth_User, parts2.get((int)0));
					headers.set(RQit.Php_Auth_Pw, parts2.get((int)1));
				}
			}
			else if (test.starts_with(RQit.digest_sp) && (!server.has_key(RQit.PHP_AUTH_DIGEST))) 
			{
				headers.set(RQit.Php_Auth_Digest, auth_hdr);
			}
			else if (test.starts_with(RQit.bearer_sp)) {
				headers.set(RQit.Authorization, auth_hdr);
			}

			if (!headers.has_key(RQit.Authorization))
			{
				if (headers.has_key(RQit.Php_Auth_User))
				{
					str_rc user = headers.get(RQit.Php_Auth_User);
					str_rc pw = headers.get(RQit.Php_Auth_Pw);
					str_rc encoded; 
					str_buf basic;

					basic << user << ':' << pw;
					encoded = basic.zstr();
					encoded = str_rc::base64_encode((const unsigned char*) encoded.data(), encoded.size());
					basic << "Basic " << encoded;
					encoded  = basic.zstr();
					headers.set(RQit.Authorization, encoded);
				}
				else 
				{
					str_rc digest = headers.get(RQit.Php_Auth_Digest);
					if (digest.ok())
					{
						headers.set(RQit.Authorization, digest);
					}
				}
			}

			if (doCallback) {
				htab_rc calldata_ht;	
				htab_rw calldata(calldata_ht);

				calldata.set(RQit.step_key, POST_AUTHORIZE);
				calldata.set(RQit.headers_key, headers);
				calldata.set(RQit.server_key, server_);

				val_rc arg1(calldata);

				val_rc ret_headers;

				if (callable_fn( ret_headers, authCallback_, 1, arg1 ))
				{
					val_ptr auth(ret_headers);
					if (auth.isArray())
					{
						headers.merge(auth.zarray());
					}
				}
			}
		} 
	}
	return result;
}

htab_rc 
RequestGlobals::smoothFiles(htab_ptr names, htab_ptr types,
	htab_ptr tmp_names, htab_ptr sizes,
	htab_ptr errors, str_ptr prefix)
{
	htab_rc result;

	htab_rw files(result);

	for_key_value wk;

	for(wk.start(names); wk.ok(); wk.next())
	{
		str_buf ss;

		size_t idx = wk.index();
		ss << prefix << '.' << idx;

		str_rc fkey = ss.zstr();
		val_ptr name(wk.value());

		if (name.isString()) 
		{
			htab_rc fdata_array;
			htab_rw ftab(fdata_array);

			ftab.set(RQit.namekey, name);
			ftab.set(RQit.typekey, types.get(idx));
			ftab.set(RQit.tmp_name, tmp_names.get(idx));
			ftab.set(RQit.size_key, sizes.get(idx));
			ftab.set(RQit.error_key, errors.get(idx));
			ftab.set(RQit.key_key, fkey);

			files.push_back(fdata_array);
		}
		else if (name.isArray()) 
		{
			htab_rc parent = smoothFiles(
				names.get(idx), types.get(idx), 
				tmp_names.get(idx), sizes.get(idx),
				errors.get(idx), 
				fkey
				);

		}
	}

	return result;
}

//public 

void RequestGlobals::debug_info(htab_rw di)
{

	di.set(RQit.server_key, server_);

	di.set(RQit.get_key, get_);

	di.set(RQit.request_key, request_);

	di.set(RQit.post_key, post_);

	di.set(RQit.files_key, files_);

	di.set(RQit.verb_key, verb_);

	di.set(RQit.strict_host, strictHost_);

	di.set(RQit.spoof_key, spoof_);

	di.set(RQit.method_override, methodOverride_);
}	


void RequestGlobals::construct()
{

	val_ptr test = htab_rc::get_global(RQit.G_SERVER);

	if (test.isArray())
	{
		server_ = Hmap::newFromArray(test);
	}
	else {
		//debug->line("No SERVER superglobal");
		server_= Hmap::new_hmap();
	}
	
	test = htab_rc::get_global(RQit.G_GET);
	if (test.isArray())
	{
		get_ = Hmap::newFromArray(test);
	}
	else {
		//debug->line("No GET superglobal");
		get_ = Hmap::new_hmap();
	}

	test = htab_rc::get_global(RQit.G_REQUEST);
	if (test.isArray())
	{
		request_ = Hmap::newFromArray(test);
	}
	else {
		//debug->line("No REQUEST superglobal");
		request_ = get_;
	}


	test = htab_rc::get_global(RQit.G_POST);
	if (test.isArray())
	{ 
		post_ = Hmap::newFromArray(test);
	}
	else {
		//debug->line("No POST superglobal");
		post_ = Hmap::new_hmap();
	}

	test = htab_rc::get_global(RQit.G_FILES);
	if (test.isArray())
	{
		files_ = Hmap::newFromArray(test);
	}
	else {
		//debug->line("No FILES superglobal");
		files_ = Hmap::new_hmap();
	}
	
	
	verb_ = 0;
	strictHost_ = true;
	spoof_ = false;
	methodOverride_ = false;
	
}

htab_rc 
RequestGlobals::getAcceptableContent()
{
	return getQualityHeader(RQit.HTTP_ACCEPT, RQit.accept);
}

htab_rc 
RequestGlobals::getBasicAuth()
{
	htab_rc result;

	htab_ptr server(readServer());

	val_ptr uname = server.get(RQit.PHP_AUTH_USER);
	val_ptr upwd = server.get(RQit.PHP_AUTH_PW);

	if (uname.isNull() || upwd.isNull())
	{
		return result;
	}

	htab_rw data(result);

	data.set(RQit.username, uname);
	data.set(RQit.password, upwd);

	return result;
}

str_rc 
RequestGlobals::getBestAccept()
{
	return getBestQuality(getAcceptableContent(), RQit.accept);
}

str_rc 
RequestGlobals::getBestLanguage()
{
	return getBestQuality(getLanguages(), RQit.language);
}


str_rc 
RequestGlobals::getClientAddress(bool trustHeader)
{
	str_rc result;
	str_rc address;

	htab_ptr server(readServer());

	if (trustHeader)
	{
		address = server.get(RQit.HTTP_X_FORWARDED_FOR);

		if (address.isNull())
		{
			address = server.get(RQit.HTTP_CLIENT_IP);
		}
	}

	if (address.isNull())
	{
		address = server.get(RQit.REMOTE_ADDR);
	}

	if (address.size() <= 0) {
		return result;
	}

	result = address;
	if (address.find(',') >= 0) 
	{
		htab_rc parts = explode(RQit.comma_key, address, 2);
		htab_ptr pa(parts);

		val_ptr p1 = pa.get(zend_long(0));
		if (p1.isString()) {
			result = p1.zstr();
		}
	}
	return result;
	
}

str_rc 
RequestGlobals::getBestCharset()
{
	htab_rc sets = getClientCharsets();

	return getBestQuality(sets, RQit.charset);
}

htab_rc
RequestGlobals::getClientCharsets()
{
	return getQualityHeader(RQit.HTTP_ACCEPT_CHARSET, RQit.charset);
}

str_rc 
RequestGlobals::getContentType()
{
	htab_ptr server(readServer());

	return server.get(RQit.CONTENT_TYPE);
}

htab_rc 
RequestGlobals::getDigestAuth()
{
	htab_rc result;

	htab_ptr server(readServer());

	val_ptr dval = server.get(RQit.PHP_AUTH_DIGEST);

	if (dval.isString())
	{
		

		preg getall("#(\\w+)=(['\"]?)([^'\" ,]+)\\2#", preg::SET_ORDER, true);
		str_rc digest(dval);

		if (getall.matches(digest) > 0) {
			htab_rw auth(result);

			htab_rc results(getall.results());

			for_key_value wk;

			for(wk.start(results); wk.ok(); wk.next())
			{
				htab_ptr match(wk.value());
				str_rc skey(match.get(int(1)));
				val_rc sval(match.get(int(3)));
				auth.set(skey, sval);
			}
		}
	}
	return result;
}

str_rc 
RequestGlobals::getHTTPReferer()
{
	htab_ptr server(readServer());
	return str_rc(server.get(RQit.HTTP_REFERER));
}

val_rc
RequestGlobals::getHeader(str_ptr header)
{
	htab_ptr server(readServer());
	val_ptr value = server.get(header);
	if (!value.isNull())
	{
		return value;
	}

	str_rc name = strtr(header, RQit.hyphen_key, RQit.underscore);

	value = server.get(name);

	if (!value.isNull())
	{
		return value;
	}

	str_buf buf;
	buf << RQit.HTTP_ << name;

	name = buf.zstr();

	return server.get(name);
}

void 
RequestGlobals::setHeader(str_ptr header, val_ptr value)
{
	Hmap* server = zobj_toc<Hmap>(server_);

	server->set(header,value);
}

// a  pain to provide this
static str_rc make_key(str_ptr endstr)
{
	// turn into "words"
	str_rc trans = zpp::strtr(endstr, RQit.underscore, RQit.blank_key);

	trans.lowercase();

	str_rc trans2(zpp::ucwords(trans));

	// join the words
	trans = zpp::strtr(trans2, RQit.blank_key, RQit.hyphen_key);
	return trans;
} 

htab_rc
RequestGlobals::getHeaders()
{
	htab_rc headers;
	htab_rw hw(headers);

	htab_rc contentHeaders;
	htab_rw content(contentHeaders);

	content.set(RQit.CONTENT_TYPE, true);
	content.set(RQit.CONTENT_LENGTH, true);
	content.set(RQit.CONTENT_MD5, true);

	for_key_value wk;

	htab_ptr server(readServer());

	for(wk.start(server); wk.ok(); wk.next())
	{
		val_ptr value(wk.value());
		val_ptr name(wk.key());

		str_rc np(name.zstr());

		if (np.starts_with(RQit.HTTP_))
		{
			str_rc hkey = make_key(np.substr(5));
			hw.set(hkey, value);
			continue;
		}

		np.uppercase();

		if (content.has_key(np))
		{
			str_rc hkey = make_key(np);
			hw.set(hkey, value);
		}
	}

	htab_rc authHeaders = resolveAuthorizationHeaders();

	hw.merge(authHeaders);

	return headers;
}

str_rc 
RequestGlobals::getOrigin()
{	
	str_rc result;
	if (origin_.size())
	{
		result = origin_;
		return result;
	}

	str_buf buf;
	str_rc scheme = getScheme();
	str_rc host = getHttpHost();

	buf << scheme << "://" << host;
	result = buf.zstr();

	return result;

}

str_rc 
RequestGlobals::getHttpHost()
{
	str_rc result;

	htab_ptr server(readServer());

	if (host_.size())
	{
		return host_; //cached
	}
	str_rc host = server.get(RQit.HTTP_HOST);
#ifdef DBG_REQUEST_GLOBALS
	DebugLog* log = DebugLog::cpp_global();
	log->dump("getHttpHost", host);
#endif
	if (!host.size())
	{
		host = server.get(RQit.SERVER_NAME);
#ifdef DBG_REQUEST_GLOBALS
	log->dump("getHttpHost-2", host);
#endif
		if (!host.size())
		{
			host = server.get(RQit.SERVER_ADDR);
#ifdef DBG_REQUEST_GLOBALS
			log->dump("getHttpHost-3", host);
#endif

		}
	}


	if (host.size() && strictHost_) 
	{
		//host.trim();
#ifdef DBG_REQUEST_GLOBALS
	log->dump("getHttpHost-host-trim", host);
#endif
		//host.lowercase();
		str_ptr empty = str_ptr::empty_str();


		str_rc test(host.data());
#ifdef DBG_REQUEST_GLOBALS
	log->dump("getHttpHost-host-lowercase", test);
#endif
		str_rc test1;
		if (test.find(':') >= 0) 
		{
			// eliminate port :digits
			preg rex1("/:[[:digit:]]+$/"); 

			test1 = test.data();
			test = rex1.replace(empty.data(), test1); 
			//showstr("after rex replace", host);
		}
		
		// Eliminate allowed
		preg rex2("/[a-z0-9-]+\\.?/");
		test1 = test.data();
		test = rex2.replace(empty.data(), test1); 
#ifdef DBG_REQUEST_GLOBALS
	log->dump("getHttpHost replace result", test);
#endif
		//showstr("test empty", test);
		if (test.size() > 0) 
		{
#ifdef DBG_REQUEST_GLOBALS
	log->line("Exception");
#endif
			error_return  bad;
			bad.error() << "Invalid name " << host;
			bad.throw_errors();
		}
		else {
			result = host;
		}
	}
#ifdef DBG_REQUEST_GLOBALS
	log->dump("getHttpHost-return", result);
#endif
	host_ = result;
	return result;
}

val_rc
RequestGlobals::getJsonRawBody(bool asArray)
{
	val_rc result;

	str_rc raw = getRawBody();

	if (raw.size())
	{
		result = json_decode(raw, asArray);
	}
	return result;
}

htab_rc 
RequestGlobals::getLanguages()
{
	return getQualityHeader(RQit.HTTP_ACCEPT_LANGUAGE, RQit.language);
}

int RequestGlobals::getMethod()
{
	if (verb_ > 0)
		return verb_;

	htab_ptr server(readServer());

	val_ptr method = server.get(RQit.REQUEST_METHOD);

	if (method.isNull())
	{
		return html::V_GET;
	}

	verb_ = Route::getVerbInt(method.zstr());

	if (verb_ == html::V_POST)
	{
		htab_ptr req(Hmap::map_htab(request_));

		str_rc test = getHeader(RQit.X_HTTP_METHOD_OVERRIDE);

		if (!test.size() && methodOverride_)
		{
			test = req.get(RQit._method);
		}
		if (test.size())
		{
			test.uppercase();
			verb_ = Route::getVerbInt(test);
		}
	}
	return verb_;
}

bool RequestGlobals::getMethodOverride()
{
	return methodOverride_;
}

void RequestGlobals::setMethodOverride(bool value)
{
	methodOverride_ = value;
}

int 
RequestGlobals::getPort()
{
	htab_ptr server(readServer());
	zend_long result = 0;

	str_rc host = server.get(RQit.HTTP_HOST);
	if (host.size())
	{
		int pos = host.rfind(':');
		if (pos >= 0) {
			str_rc sport = host.substr(pos+1);
			sport.getLong(result);
			return result;
		}
	}
	else {
		host = server.get(RQit.SERVER_PORT);
		if (host.size())
		{
			host.getLong(result);
			return result;
		}
	}
	str_rc scheme = getScheme();

	if (zs_cmp_ci(scheme, RQit.https)==0)
	{
		return 443;
	}
	else {
		return 80;
	}
}

str_rc
RequestGlobals::getRawBody()
{
	if (!body_.size())
	{
		body_ = file_content(RQit.php_input);
	}
	return body_;
}

str_rc
RequestGlobals::getScheme()
{
	htab_ptr server(readServer());

#ifdef DBG_REQUEST_GLOBALS
	DebugLog* log = DebugLog::cpp_global();
	log->dump("server arrayptr", server);
#endif
	str_rc scheme = server.get(RQit.HTTPS);
#ifdef DBG_REQUEST_GLOBALS
	log->dump("scheme", scheme);
#endif
	if (scheme.size())
	{
		if (zs_cmp_ci(scheme, RQit.off_key) != 0)
		{
			scheme = RQit.https;
		}
		else {
			scheme = RQit.http;
		}
	}
#ifdef DBG_REQUEST_GLOBALS
	log->dump("scheme", scheme);
#endif
	return scheme;
}

str_rc 
RequestGlobals::getServerAddress()
{
	htab_ptr server(readServer());

	str_rc serverAddr = server.get(RQit.SERVER_ADDR);
	if (serverAddr.size()) {
		return str_rc(serverAddr);
	}

	return gethostbyname(RQit.localhost);
}

str_rc 
RequestGlobals::getServerName()
{
	htab_ptr server(readServer());

	str_rc name = server.get(RQit.SERVER_NAME);

	if (name.isNull())
	{
		name = RQit.localhost;
	}
	return name;
}

bool RequestGlobals::getSpoofMethod()
{
	return spoof_;
}

void RequestGlobals::setSpoofMethod(bool val)
{
	spoof_ = val;
}

str_rc 
RequestGlobals::getURI(bool onlyPath)
{	
	str_rc result;

	htab_ptr server(readServer());

	result = server.get(RQit.REQUEST_URI);

#ifdef DBG_REQUEST_GLOBALS
	DebugLog* log = DebugLog::cpp_global();
	log->dump("RequestGlobals getURI-1", result);

#endif

	if (result.isNull())
	{
		result = str_empty();
	}
	else {
		size_t slen = result.size();
		if (onlyPath && slen)
		{
			//zend_printf("only_path %lx\n", slen);
			int qpos = result.find('?');

			if (qpos >= 0) {
				//showstr("uri is", uri);
				//zend_printf("qpos %ld\n", qpos);
				result = result.substr(0,qpos);
			}
		}
	}
#ifdef DBG_REQUEST_GLOBALS
	log->dump("RequestGlobals getURI-2", result);
#endif
	return result;
}

htab_rc 
RequestGlobals::getUploadedFiles(bool onlySuccess, bool namekeys)
{
	htab_rc result;
	htab_rw fileobjs(result);

	htab_ptr files(Hmap::map_htab(files_));

	if (files.size())
	{
		htab_walk wk;

		auto key = wk.key();
		auto value = wk.value();

		str_ptr typekey = RQit.typekey;
		str_ptr tmp_name = RQit.tmp_name;
		str_ptr sizekey = RQit.size_key;
		str_ptr errorkey = RQit.error_key;
		str_ptr namekey = RQit.namekey;
		str_ptr keykey = RQit.key_key;

		for(wk.start(files); wk.ok(); wk.next())
		{
			htab_ptr input(value.zarray());

			val_ptr nv = input.get(namekey);

			int input_error = val_ptr(input.get(errorkey)).zlong();

			str_rc prefix = key.zstr();

			//showstr("prefix", prefix);

			if (nv.isArray())
			{

				htab_rc smooth = smoothFiles(
									 nv.zarray(),
									 input.get(typekey),
									 input.get(tmp_name),
									 input.get(sizekey),
									 input.get(errorkey),
									 prefix
								  );
				htab_walk sh;
				auto shval = sh.value();

				for(sh.start(smooth); sh.ok(); sh.next())
				{
					htab_ptr file = shval.zarray();
					int error_val = val_ptr(file.get(errorkey)).zlong();

					if ((!onlySuccess) || (error_val == Upload::ERROR_OK))
					{
						htab_rc dataFile;
						htab_rw fdata(dataFile);

						fdata.set(namekey, file.get(namekey));
						fdata.set(typekey, file.get(typekey));
						fdata.set(tmp_name, file.get(tmp_name));
						fdata.set(sizekey, file.get(sizekey));
						fdata.set(errorkey, file.get(errorkey));

						str_rc fkey = file.get(keykey);
						obj_rc file_obj = makeFile(fdata, fkey);

						if (namekeys) {
							fileobjs.set(fkey, file_obj);
						}
						else {
							fileobjs.push_back(file_obj);
						}
					}
				}
			}
			else if ( (!onlySuccess) || (input_error == Upload::ERROR_OK))
			{
				obj_rc file_obj = makeFile(input, prefix);
				if (namekeys) {
					fileobjs.set(prefix, file_obj);
				}
				else {
					fileobjs.push_back(file_obj);
				}
			}
		}
	}
	return result;
}

str_rc 
RequestGlobals::getUserAgent()
{
	htab_ptr server(readServer());
	return server.get(RQit.HTTP_USER_AGENT);
}

bool 
RequestGlobals::hasFiles()
{
	return numFiles(true) > 0;
}

bool 
RequestGlobals::hasHeader(str_ptr header)
{
	str_rc hval = getHeader(header);
	return hval.ok();
}

bool 
RequestGlobals::hasQuery(str_ptr key)
{
	htab_rc query(Hmap::map_htab(get_));

	return val_ptr(query.get(key)).ok();
}

bool 
RequestGlobals::hasRequest(str_ptr key)
{
	htab_ptr req(Hmap::map_htab(request_));

	return val_ptr(req.get(key)).ok();
}

bool 
RequestGlobals::isAjax()
{
	htab_ptr server(readServer());

	str_rc check = server.get(RQit.HTTP_X_REQUESTED_WITH);

	return (check.size() && (zs_cmp_ci(check, RQit.XMLHttpRequest)==0));
}

bool 
RequestGlobals::isMethod(val_ptr methods, bool strict)
{
	int verb = getMethod();

	if (methods.isString()) {
		str_rc vstr = methods.zstr();
		int test = Route::getVerbInt(vstr);
		if (test == verb) {
			return true;
		}
		if (strict && (test == 0)) 
		{
			zend_throw_error(zend_ce_exception,"Invalid HTTP constant %s", vstr.data());
		}
		return false;
	}

	if (methods.isLong()) {
		return (methods.zlong() == verb);
	}

	if (methods.isArray())
	{
		htab_ptr list(methods.zarray());

		htab_walk wk;
		auto method = wk.value();

		for(wk.start(list); wk.ok(); wk.next())
		{
			if (isMethod(method, strict)) {
				return true;
			}
		}
		return false;
	}

	if (strict) 
	{
		zend_throw_error(zend_ce_exception,"Invalid HTTP method type %s", zend_zval_type_name(methods));
	}

	return false;
}

bool 
RequestGlobals::isOptions()
{
	return getMethod() == html::V_OPTIONS;
}

bool 
RequestGlobals::isPatch()
{
	return getMethod() == html::V_PATCH;
}

bool 
RequestGlobals::isPost()
{
	return getMethod() == html::V_POST;
}

bool 
RequestGlobals::isPurge()
{
	return getMethod() == html::V_PURGE;
}

bool 
RequestGlobals::isPut()
{
	return getMethod() == html::V_PUT;
}

bool 
RequestGlobals::isSecure()
{
	str_rc test = getScheme();

	if (test.size() == 0)
		return false;
	
	return zs_cmp_ci(test, RQit.https)==0;
}

bool 
RequestGlobals::isSoap()
{
	htab_ptr server(readServer());

	str_rc soap = server.get(RQit.HTTP_SOAPACTION);
	if (soap.size())
	{
		return true;
	}

	str_rc contentType = getContentType();
	str_rc test(contentType);

	if (!test.size())
	{
		return false;
	}
	return test.contains(RQit.soap_mime);
}

int 
RequestGlobals::numFiles(bool onlySuccess)
{
	htab_ptr files(Hmap::map_htab(files_));

	if (!files.size())
	{
		return 0;
	}

	int count = 0;
	htab_walk wk;
	auto value = wk.value();
	for(wk.start(files); wk.ok(); wk.next())
	{
		if (value.isArray())
		{
			htab_ptr file(value.zarray());

			val_ptr error = file.get(RQit.error_key);

			if (error.isArray()) {
				count += fileCounter(error.zarray(), onlySuccess);
				continue;
			}

			if (error.isNull() || !onlySuccess)
			{
				count++;
			}
		}
	}
	return count;
}

obj_ptr
RequestGlobals::post()
{
	return post_;
}

obj_ptr 
RequestGlobals::query()
{
	return request_;
}

obj_ptr
RequestGlobals::server()
{
	return server_;
}

htab_ptr
RequestGlobals::getPost()
{
	return Hmap::map_htab(post_);
}

htab_ptr 
RequestGlobals::getQuery()
{
	return Hmap::map_htab(request_);
}

htab_ptr
RequestGlobals::getServer()
{
	return Hmap::map_htab(server_);
}

void 
RequestGlobals::setStrictHost(bool val)
{
	strictHost_ = val;
}

}; // end namespace

ZEND_METHOD(Wcc_RequestGlobals, __construct)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	cobj->construct();
}

ZEND_METHOD(Wcc_RequestGlobals, setHeader)
{
	zend_string* name;
	zval*        value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_STR(name)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	cobj->setHeader(name, value);


}

ZEND_METHOD(Wcc_RequestGlobals, getHeader)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	val_rc result = cobj->getHeader(name);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_RequestGlobals, hasHeader)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	bool result = cobj->hasHeader(name);
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_RequestGlobals, getBasicAuth)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	htab_rc result = cobj->getBasicAuth();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getServerName)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	str_rc result = cobj->getServerName();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getMethod)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	zend_long result = cobj->getMethod();
	RETURN_LONG(result);
}

ZEND_METHOD(Wcc_RequestGlobals, isAjax)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	bool result = cobj->isAjax();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_RequestGlobals, getScheme)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	str_rc result = cobj->getScheme();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getURI)
{
	bool onlypath = false;

	ZEND_PARSE_PARAMETERS_START(0, 1)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(onlypath)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	str_rc result = cobj->getURI(onlypath);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getAcceptableContent)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	htab_rc result = cobj->getAcceptableContent();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getHeaders)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	htab_rc result = cobj->getHeaders();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, hasRequest)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	bool result = cobj->hasRequest(name);
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_RequestGlobals, isMethod)
{
	zval* methods;
	bool  strict = false;

	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_ZVAL(methods)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(strict)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	bool result = cobj->isMethod(methods, strict);
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_RequestGlobals, isOptions)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	bool result = cobj->isOptions();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_RequestGlobals, isPatch)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	bool result = cobj->isPatch();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_RequestGlobals, isPost)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	bool result = cobj->isPost();
	RETURN_BOOL(result);

}

ZEND_METHOD(Wcc_RequestGlobals, isPut)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	bool result = cobj->isPut();
	RETURN_BOOL(result);

}

ZEND_METHOD(Wcc_RequestGlobals, isPurge)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	bool result = cobj->isPurge();
	RETURN_BOOL(result);

}

ZEND_METHOD(Wcc_RequestGlobals, isSecure)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	bool result = cobj->isSecure();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_RequestGlobals, numFiles)
{
	bool only_success = false;
	ZEND_PARSE_PARAMETERS_START(0, 1)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(only_success)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	zend_long result = cobj->numFiles(only_success);
	RETURN_LONG(result);
}

ZEND_METHOD(Wcc_RequestGlobals, hasFiles)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	bool result = cobj->hasFiles();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_RequestGlobals, getBestAccept)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	str_rc result = cobj->getBestAccept();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getClientAddress)
{
	bool trustForward = false;
	ZEND_PARSE_PARAMETERS_START(0, 1)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(trustForward)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	str_rc result = cobj->getClientAddress(trustForward);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, hasQuery)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	bool result = cobj->hasQuery(name);
	RETURN_BOOL(result);	
}

ZEND_METHOD(Wcc_RequestGlobals, getJsonRawBody)
{
	bool useArray = false;
	ZEND_PARSE_PARAMETERS_START(0, 1)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(useArray)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	val_rc result = cobj->getJsonRawBody(useArray);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getRawBody)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	str_rc result = cobj->getRawBody();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getLanguages)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	htab_rc result = cobj->getLanguages();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getUserAgent)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	str_rc result = cobj->getUserAgent();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getBestCharset)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	str_rc result = cobj->getBestCharset();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getClientCharsets)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	htab_rc result = cobj->getClientCharsets();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getBestLanguage)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	str_rc result = cobj->getBestLanguage();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getPort)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	zend_long result = cobj->getPort();
	RETURN_LONG(result);
}

ZEND_METHOD(Wcc_RequestGlobals, getContentType)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	str_rc result = cobj->getContentType();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getHttpHost)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	str_rc result = cobj->getHttpHost();
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_RequestGlobals, getOrigin)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	str_rc result = cobj->getOrigin();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, post)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	obj_ptr result = cobj->post();
	result.copy_zv(return_value);

}

ZEND_METHOD(Wcc_RequestGlobals, query)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	obj_ptr result = cobj->query();
	//showmem("getQuery result", result);
	result.copy_zv(return_value);

}

ZEND_METHOD(Wcc_RequestGlobals, server)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	obj_ptr result = cobj->server();
	//showmem("getQuery result", result);
	result.copy_zv(return_value);

}

ZEND_METHOD(Wcc_RequestGlobals, getPost)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	htab_ptr result = cobj->getPost();
	result.copy_zv(return_value);

}

ZEND_METHOD(Wcc_RequestGlobals, getQuery)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	htab_ptr result = cobj->getQuery();
	//showmem("getQuery result", result);
	result.copy_zv(return_value);

}

ZEND_METHOD(Wcc_RequestGlobals, getServer)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	htab_ptr result = cobj->getServer();
	//showmem("getQuery result", result);
	result.copy_zv(return_value);

}

ZEND_METHOD(Wcc_RequestGlobals, getUploadedFiles)
{
	bool only_success = false;
	bool namekeys = false;

	ZEND_PARSE_PARAMETERS_START(0, 2)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(only_success)
	Z_PARAM_BOOL(namekeys)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	htab_rc result = cobj->getUploadedFiles(only_success, namekeys);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getDigestAuth)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	htab_rc result = cobj->getDigestAuth();
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_RequestGlobals, getHTTPReferer)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	str_rc result = cobj->getHTTPReferer();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getServerAddress)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	str_rc result = cobj->getServerAddress();
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_RequestGlobals, isSoap)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	bool result = cobj->isSoap();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_RequestGlobals, getMethodOverride)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	bool result = cobj->getMethodOverride();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_RequestGlobals, setMethodOverride)
{
	bool value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_BOOL(value)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	cobj->setMethodOverride(value);
}

ZEND_METHOD(Wcc_RequestGlobals, getSpoofMethod)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	bool result = cobj->getSpoofMethod();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_RequestGlobals, setSpoofMethod)
{
	bool value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_BOOL(value)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	cobj->setSpoofMethod(value);
}

ZEND_METHOD(Wcc_RequestGlobals, setStrictHost)
{
	bool value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_BOOL(value)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	cobj->setStrictHost(value);
}

ZEND_METHOD(Wcc_RequestGlobals, setAuthCallback)
{
	zval* value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	cobj->setAuthCallback(value);
}

PHP_MINIT_FUNCTION(RequestGlobals_reg)
{
	auto ce = register_class_Wcc_RequestGlobals();

	RequestGlobals::omg.classEntry(ce);

	STATE_INIT_ADD(RQit)
	
	return SUCCESS;
}

//request_globals.cpp
#endif