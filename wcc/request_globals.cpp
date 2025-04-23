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


zstr_mgr 
gethostbyname(zstr_user s)
{
    fn_call_args<1>  fn;
    ZVAL_STR(fn.argsptr(), s);
    fn.set_fname(RQit.gethostbyname);
    return fn.call_fn();
}

zval_mgr 
finfo_open(int infoflags)
{
    fn_call_args<1>  fn;
    ZVAL_LONG(fn.argsptr(), infoflags);
    fn.set_fname(RQit.finfo_open);
    return fn.call_fn();
}

zval_mgr 
finfo_file(zval_mgr& finfo, zstr_user path)
{
    fn_call_args<2>  fn;
    zval* pargs = fn.argsptr();

    ZVAL_COPY(pargs, finfo);
    ZVAL_STR(pargs+1, path);
    fn.set_fname(RQit.finfo_file);
    return fn.call_fn();
}

zval_mgr 
finfo_close(zval_mgr& finfo)
{
    fn_call_args<1>  fn;
    ZVAL_COPY(fn.argsptr(), finfo);
    fn.set_fname(RQit.finfo_close);
    return fn.call_fn();
}

bool is_uploaded_file(zstr_user path)
{
    fn_call_args<1>  fn;
    ZVAL_STR(fn.argsptr(), path);
    fn.set_fname(RQit.is_uploaded_file);
    zval_mgr result = fn.call_fn();
    return zval_user(result).isTrue();
}

bool
 move_uploaded_file(zstr_user from, zstr_user to)
{
	fn_call_args<2>  fn;
	zval* pargs = fn.argsptr();
	ZVAL_STR(pargs, from);
	ZVAL_STR(pargs+1, to);
	fn.set_fname(RQit.move_uploaded_file);
    zval_mgr result = fn.call_fn();
    return zval_user(result).isTrue();
}

//protected
htab_read RequestGlobals::readServer()
{
	return Hmap::map_htab(server_);
}

//protected
int 
RequestGlobals::fileCounter(htab_read data, bool onlySuccessful)
{
	int result = 0;

	for_key_value wk;

	for(wk.start(data); wk.ok(); wk.next())
	{
		zval_user value(wk.value());
		if (value.isArray())
		{
			htab_read fsub(value);

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

zobj_mgr
RequestGlobals::makeFile(htab_read fdata, zstr_user name)
{
	zobj_mgr result = FileUpload::omg.new_zobj();
	FileUpload* fup = zobj_toc<FileUpload>(result);
	fup->construct(fdata, name);

	return result;
}

zstr_mgr 
RequestGlobals::getBestQuality(htab_read parts, zstr_user name)
{
	int i = 0;
	double quality = 0.0;
	double more = 0.0;

	zstr_mgr  result;

	for_key_value wk;
	

	for(wk.start(parts); wk.ok(); wk.next(), i++)
	{
		zval_user value(wk.value());

		if (!value.isArray()) continue;

		htab_read accept(value);

		zval_user dval( accept.get(RQit.quality) );
		zval_user sval( accept.get(name) );

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

htab_mgr 
RequestGlobals::getQualityHeader(zstr_user key, zstr_user name)
{
	htab_mgr result;
	htab_write qh(result);

	htab_read server(readServer());

	zstr_user data = server.get(key);

	if (data.isNull())
	{
		return result;
	}

	zval_mgr parts_split = preg_split("/,\\s*/", data, -1, preg::SPLIT_NO_EMPTY);
	zval_user parts(parts_split);

	if (!parts.isArray())
	{
		return result;
	}

	preg 	data_split("/\\s*;\\s*/", preg::SPLIT_NO_EMPTY);

	for_key_value wk;

	for(wk.start(parts.zarray()); wk.ok(); wk.next())
	{
		zval_user part(wk.value());

		htab_mgr headerParts;
		htab_write collect(headerParts);

		zval_mgr rs = data_split.splits(part.zstr(), -1);
		zval_user sc_split(rs);

		if (!sc_split.isArray()) continue;

		for_key_value w2;

		
		for(w2.start(sc_split.zarray()); w2.ok(); w2.next())
		{
			zval_user headerPartVal(w2.value());

			zstr_user  headerPart = headerPartVal.zstr();

			if (headerPart.find('=') >= 0) 
			{
				htab_mgr tuple = explode(RQit.equals_key, headerPart, 2);
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

htab_mgr 
RequestGlobals::resolveAuthorizationHeaders()
{
	zval_user auth_call(authCallback_);

	bool doCallback = auth_call.ok();
	htab_mgr result;
	htab_write headers(result);

	if (doCallback) {
		htab_mgr arg_mgr;	
		htab_write args(arg_mgr);

		args.set(RQit.step_key, PRE_AUTHORIZE);
		args.set(RQit.server_key, server_);

		zval_mgr arg1(arg_mgr);

		zval_mgr ret_headers;

		if (callable_fn( ret_headers, authCallback_, 1, arg1 ))
		{
			zval_user auth(ret_headers);
			if (auth.isArray())
			{
				headers.merge(auth.zarray());
			}
		}
	}

	htab_read server(readServer());
	zval_user user = server.get(RQit.PHP_AUTH_USER);
	zval_user apw =  server.get(RQit.PHP_AUTH_PW);

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

		zstr_user auth_hdr = server.get(RQit.HTTP_AUTHORIZATION);
		if (auth_hdr.isNull())
		{
			auth_hdr = server.get(RQit.REDIRECT_HTTP_AUTHORIZATION);
		}
		if ( auth_hdr.size() ) 
		{
			zstr_mgr lc_auth_hdr = auth_hdr.to_lower();

			zstr_user test(lc_auth_hdr);

			if (test.starts_with(RQit.basic_sp)) 
			{
				zstr_mgr decode = zstr_mgr::base64_decode((const unsigned char*)auth_hdr.data() + 6, 
																auth_hdr.size()-6);

				zval_mgr  excode = explode(RQit.colon_key, decode, 2);
				zval_user parts(excode);

				if (parts.isArray())
				{
					htab_read parts2(parts);
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
					zstr_user user = headers.get(RQit.Php_Auth_User);
					zstr_user pw = headers.get(RQit.Php_Auth_Pw);
					zstr_mgr encoded; 
					zstr_buffer basic;

					basic << user << ':' << pw;
					encoded = basic.zstr();
					encoded = zstr_mgr::base64_encode((const unsigned char*) encoded.data(), encoded.size());
					basic << "Basic " << encoded;
					encoded  = basic.zstr();
					headers.set(RQit.Authorization, encoded);
				}
				else 
				{
					zstr_user digest = headers.get(RQit.Php_Auth_Digest);
					if (digest.ok())
					{
						headers.set(RQit.Authorization, digest);
					}
				}
			}

			if (doCallback) {
				htab_mgr calldata_ht;	
				htab_write calldata(calldata_ht);

				calldata.set(RQit.step_key, POST_AUTHORIZE);
				calldata.set(RQit.headers_key, headers);
				calldata.set(RQit.server_key, server_);

				zval_mgr arg1(calldata);

				zval_mgr ret_headers;

				if (callable_fn( ret_headers, authCallback_, 1, arg1 ))
				{
					zval_user auth(ret_headers);
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

htab_mgr 
RequestGlobals::smoothFiles(htab_read names, htab_read types,
	htab_read tmp_names, htab_read sizes,
	htab_read errors, zstr_user prefix)
{
	htab_mgr result;

	htab_write files(result);

	for_key_value wk;

	for(wk.start(names); wk.ok(); wk.next())
	{
		zstr_buffer ss;

		size_t idx = wk.index();
		ss << prefix << '.' << idx;

		zstr_mgr fkey = ss.zstr();
		zval_user name(wk.value());

		if (name.isString()) 
		{
			htab_mgr fdata_array;
			htab_write ftab(fdata_array);

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
			htab_mgr parent = smoothFiles(
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

void RequestGlobals::debug_info(htab_write di)
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
	server_ = Hmap::newFromArray(htab_mgr::get_global(RQit.G_SERVER));
	get_ = Hmap::newFromArray(htab_mgr::get_global(RQit.G_GET));

	// Using G_REQUEST isn't recommended.
	zval_user gval = htab_mgr::get_global(RQit.G_REQUEST);
	if (gval.isArray())
	{ 
		request_ = Hmap::newFromArray(gval);
	}
	else {
		request_ = Hmap::new_hmap();
	}

	post_ = Hmap::newFromArray(htab_mgr::get_global(RQit.G_POST));
	files_ = Hmap::newFromArray(htab_mgr::get_global(RQit.G_FILES));

	verb_ = 0;
	strictHost_ = true;
	spoof_ = false;
	methodOverride_ = false;
}

htab_mgr 
RequestGlobals::getAcceptableContent()
{
	return getQualityHeader(RQit.HTTP_ACCEPT, RQit.accept);
}

htab_mgr 
RequestGlobals::getBasicAuth()
{
	htab_mgr result;

	htab_read server(readServer());

	zval_user uname = server.get(RQit.PHP_AUTH_USER);
	zval_user upwd = server.get(RQit.PHP_AUTH_PW);

	if (uname.isNull() || upwd.isNull())
	{
		return result;
	}

	htab_write data(result);

	data.set(RQit.username, uname);
	data.set(RQit.password, upwd);

	return result;
}

zstr_mgr 
RequestGlobals::getBestAccept()
{
	return getBestQuality(getAcceptableContent(), RQit.accept);
}

zstr_mgr 
RequestGlobals::getBestLanguage()
{
	return getBestQuality(getLanguages(), RQit.language);
}


zstr_mgr 
RequestGlobals::getClientAddress(bool trustHeader)
{
	zstr_mgr result;
	zstr_user address;

	htab_read server(readServer());

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
		htab_mgr parts = explode(RQit.comma_key, address, 2);
		htab_read pa(parts);

		zval_user p1 = pa.get(zend_long(0));
		if (p1.isString()) {
			result = p1.zstr();
		}
	}
	return result;
	
}

zstr_mgr 
RequestGlobals::getBestCharset()
{
	htab_mgr sets = getClientCharsets();

	return getBestQuality(sets, RQit.charset);
}

htab_mgr
RequestGlobals::getClientCharsets()
{
	return getQualityHeader(RQit.HTTP_ACCEPT_CHARSET, RQit.charset);
}

zstr_mgr 
RequestGlobals::getContentType()
{
	htab_read server(readServer());

	return server.get(RQit.CONTENT_TYPE);
}

htab_mgr 
RequestGlobals::getDigestAuth()
{
	htab_mgr result;

	htab_read server(readServer());

	zval_user dval = server.get(RQit.PHP_AUTH_DIGEST);

	if (dval.isString())
	{
		

		preg getall("#(\\w+)=(['\"]?)([^'\" ,]+)\\2#", preg::SET_ORDER, true);
		zstr_mgr digest(dval);

		if (getall.matches(digest) > 0) {
			htab_write auth(result);

			htab_mgr results(getall.results());

			for_key_value wk;

			for(wk.start(results); wk.ok(); wk.next())
			{
				htab_read match(wk.value());
				zstr_user skey(match.get(int(1)));
				zval_user sval(match.get(int(3)));
				auth.set(skey, sval);
			}
		}
	}
	return result;
}

zstr_mgr 
RequestGlobals::getHTTPReferer()
{
	htab_read server(readServer());
	return zstr_mgr(server.get(RQit.HTTP_REFERER));
}

zval_mgr
RequestGlobals::getHeader(zstr_user header)
{
	htab_read server(readServer());
	zval_user value = server.get(header);
	if (!value.isNull())
	{
		return value;
	}

	zstr_mgr name = strtr(header, RQit.hyphen_key, RQit.underscore);

	value = server.get(name);

	if (!value.isNull())
	{
		return value;
	}

	zstr_buffer buf;
	buf << RQit.HTTP_ << name;

	name = buf.zstr();

	return server.get(name);
}

void 
RequestGlobals::setHeader(zstr_user header, zval_user value)
{
	Hmap* server = zobj_toc<Hmap>(server_);

	server->set(header,value);
}

// a  pain to provide this
static zstr_mgr make_key(zstr_user endstr)
{
	// turn into "words"
	zstr_mgr trans = zpp::strtr(endstr, RQit.underscore, RQit.blank_key);
	trans = zstr_user(trans).to_lower();

	trans = zpp::ucwords(trans);

	// join the words
	trans = zpp::strtr(trans,RQit.blank_key, RQit.hyphen_key);
	return trans;
} 

htab_mgr
RequestGlobals::getHeaders()
{
	htab_mgr headers;
	htab_write hw(headers);

	htab_mgr contentHeaders;
	htab_write content(contentHeaders);

	content.set(RQit.CONTENT_TYPE, true);
	content.set(RQit.CONTENT_LENGTH, true);
	content.set(RQit.CONTENT_MD5, true);

	for_key_value wk;

	htab_read server(readServer());

	for(wk.start(server); wk.ok(); wk.next())
	{
		zval_user value(wk.value());
		zval_user name(wk.key());

		zstr_user np(name.zstr());

		if (np.starts_with(RQit.HTTP_))
		{
			zstr_mgr hkey = make_key(np.substr(5));
			hw.set(hkey, value);
			continue;
		}

		np = np.to_upper();
		if (content.has_key(np))
		{
			zstr_mgr hkey = make_key(np);
			hw.set(hkey, value);
		}
	}

	htab_mgr authHeaders = resolveAuthorizationHeaders();

	hw.merge(authHeaders);

	return headers;
}

zstr_mgr 
RequestGlobals::getHttpHost()
{
	zstr_mgr result;

	htab_read server(readServer());
	zstr_user host = server.get(RQit.HTTP_HOST);

	if (!host.size())
	{
		host = server.get(RQit.SERVER_NAME);

		if (!host.size())
		{
			host = server.get(RQit.SERVER_ADDR);
		}
	}
	result = host;

	if (host.size() && strictHost_) 
	{
		// regulations want lowercase
		result = host.trim();
		//showstr("after trim", host);
		host = result;
		result = host.to_lower();

		host = result;

		//showstr("host2", host);
		if (host.find(':') >= 0) 
		{
			// eliminate port :digits
			preg rex1("/:[[:digit:]]+$/"); 

			result = rex1.replace("", host); 
			host = result;
			//showstr("after rex replace", host);
		}
		
		// Eliminate allowed
		preg rex2("/[a-z0-9-]+\\.?/");

		zstr_mgr test = rex2.replace("", host); 
		//showstr("test empty", test);
		if (test.size() > 0) 
		{
			zend_throw_error(zend_ce_exception, "Invalid host %s", host.data());
		}
	}

	return result;
}

zval_mgr
RequestGlobals::getJsonRawBody(bool asArray)
{
	zval_mgr result;

	zstr_mgr raw = getRawBody();
	zstr_user test(raw);

	if (test.size())
	{
		result = json_decode(test, asArray);
	}
	return result;
}

htab_mgr 
RequestGlobals::getLanguages()
{
	return getQualityHeader(RQit.HTTP_ACCEPT_LANGUAGE, RQit.language);
}

int RequestGlobals::getMethod()
{
	if (verb_ > 0)
		return verb_;

	htab_read server(readServer());

	zval_user method = server.get(RQit.REQUEST_METHOD);

	if (method.isNull())
	{
		return html::V_GET;
	}

	verb_ = Route::getVerbInt(method.zstr());

	if (verb_ == html::V_POST)
	{
		htab_read req(Hmap::map_htab(request_));

		zstr_mgr override = getHeader(RQit.X_HTTP_METHOD_OVERRIDE);

		zstr_user test(override);

		if (!test.size() && methodOverride_)
		{
			test = req.get(RQit._method);
		}
		if (test.size())
		{
			override = test.to_upper();

			verb_ = Route::getVerbInt(override);
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
	htab_read server(readServer());

	zstr_user host = server.get(RQit.HTTP_HOST);
	if (host.size())
	{
		int pos = host.rfind(':');
		if (pos >= 0) {
			zstr_mgr sport = host.substr(pos+1);
			return zstr_user(sport).getLong();
		}
	}
	else {
		host = server.get(RQit.SERVER_PORT);
		if (host.size())
		{
			return host.getLong();
		}
	}
	zstr_user scheme = getScheme();

	if (zs_cmp_ci(scheme, RQit.https)==0)
	{
		return 443;
	}
	else {
		return 80;
	}
}

zstr_mgr
RequestGlobals::getRawBody()
{
	if (!body_.size())
	{
		body_ = FTAB.file_get_contents.call(RQit.php_input);
	}
	return body_;
}

zstr_user
RequestGlobals::getScheme()
{
	htab_read server(readServer());

	zstr_user scheme = server.get(RQit.HTTPS);
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
	return scheme;
}

zstr_mgr 
RequestGlobals::getServerAddress()
{
	htab_read server(readServer());

	zstr_user serverAddr = server.get(RQit.SERVER_ADDR);
	if (serverAddr.size()) {
		return zstr_mgr(serverAddr);
	}

	return gethostbyname(zstr_user(RQit.localhost));
}

zstr_mgr 
RequestGlobals::getServerName()
{
	htab_read server(readServer());

	zstr_mgr name = server.get(RQit.SERVER_NAME);

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

zstr_mgr 
RequestGlobals::getURI(bool onlyPath)
{	
	zstr_mgr result;

	htab_read server(readServer());

	result = server.get(RQit.REQUEST_URI);

	zstr_user uri (result);

	if (uri.isNull())
	{
		result = zstr_empty();
	}
	else {
		size_t slen = uri.size();
		if (onlyPath && slen)
		{
			//zend_printf("only_path %lx\n", slen);
			int qpos = uri.find('?');
			if (qpos >= 0) {
				//showstr("uri is", uri);
				//zend_printf("qpos %ld\n", qpos);
				result = uri.substr(0,qpos);
			}
		}
	}

	return result;
}

htab_mgr 
RequestGlobals::getUploadedFiles(bool onlySuccess, bool namekeys)
{
	htab_mgr result;
	htab_write fileobjs(result);

	htab_read files(Hmap::map_htab(files_));

	if (files.size())
	{
		htab_walk wk;

		auto key = wk.key();
		auto value = wk.value();

		zstr_user typekey = RQit.typekey;
		zstr_user tmp_name = RQit.tmp_name;
		zstr_user sizekey = RQit.size_key;
		zstr_user errorkey = RQit.error_key;
		zstr_user namekey = RQit.namekey;
		zstr_user keykey = RQit.key_key;

		for(wk.start(files); wk.ok(); wk.next())
		{
			htab_read input(value);

			zval_user nv = input.get(namekey);

			int input_error = zval_user(input.get(errorkey)).zlong();

			zstr_mgr prefix = key.zstr();

			//showstr("prefix", prefix);

			if (nv.isArray())
			{

				htab_mgr smooth = smoothFiles(
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
					htab_read file = shval.zarray();
					int error_val = zval_user(file.get(errorkey)).zlong();

					if ((!onlySuccess) || (error_val == Upload::ERROR_OK))
					{
						htab_mgr dataFile;
						htab_write fdata(dataFile);

						fdata.set(namekey, file.get(namekey));
						fdata.set(typekey, file.get(typekey));
						fdata.set(tmp_name, file.get(tmp_name));
						fdata.set(sizekey, file.get(sizekey));
						fdata.set(errorkey, file.get(errorkey));

						zstr_mgr fkey = file.get(keykey);
						zobj_mgr file_obj = makeFile(fdata, fkey);

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
				zobj_mgr file_obj = makeFile(input, prefix);
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

zstr_mgr 
RequestGlobals::getUserAgent()
{
	htab_read server(readServer());
	return server.get(RQit.HTTP_USER_AGENT);
}

bool 
RequestGlobals::hasFiles()
{
	return numFiles(true) > 0;
}

bool 
RequestGlobals::hasHeader(zstr_user header)
{
	zstr_mgr hval = getHeader(header);
	return hval.ok();
}

bool 
RequestGlobals::hasQuery(zstr_user key)
{
	htab_read query(Hmap::map_htab(get_));

	return zval_user(query.get(key)).ok();
}

bool 
RequestGlobals::hasRequest(zstr_user key)
{
	htab_read req(Hmap::map_htab(request_));

	return zval_user(req.get(key)).ok();
}

bool 
RequestGlobals::isAjax()
{
	htab_read server(readServer());

	zstr_user check = server.get(RQit.HTTP_X_REQUESTED_WITH);

	return (check.size() && (zs_cmp_ci(check, RQit.XMLHttpRequest)==0));
}

bool 
RequestGlobals::isMethod(zval_user methods, bool strict)
{
	int verb = getMethod();

	if (methods.isString()) {
		zstr_user vstr = methods.zstr();
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
		htab_read list(methods.zarray());

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
	zstr_user test = getScheme();

	if (test.size() == 0)
		return false;
	
	return zs_cmp_ci(test, RQit.https)==0;
}

bool 
RequestGlobals::isSoap()
{
	htab_read server(readServer());

	zstr_user soap = server.get(RQit.HTTP_SOAPACTION);
	if (soap.size())
	{
		return true;
	}

	zstr_mgr contentType = getContentType();
	zstr_user test(contentType);

	if (!test.size())
	{
		return false;
	}
	return test.contains(RQit.soap_mime);
}

int 
RequestGlobals::numFiles(bool onlySuccess)
{
	htab_read files(Hmap::map_htab(files_));

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
			htab_read file(value);

			zval_user error = file.get(RQit.error_key);

			if (error.isArray()) {
				count += fileCounter(error, onlySuccess);
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

zobj_user
RequestGlobals::post()
{
	return post_;
}

zobj_user 
RequestGlobals::query()
{
	return request_;
}

zobj_user
RequestGlobals::server()
{
	return server_;
}

htab_read
RequestGlobals::getPost()
{
	return Hmap::map_htab(post_);
}

htab_read 
RequestGlobals::getQuery()
{
	return Hmap::map_htab(get_);
}

htab_read
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
	zval_mgr result = cobj->getHeader(name);
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
	htab_mgr result = cobj->getBasicAuth();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getServerName)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	zstr_mgr result = cobj->getServerName();
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
	zstr_user result = cobj->getScheme();
	result.return_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getURI)
{
	bool onlypath = false;

	ZEND_PARSE_PARAMETERS_START(0, 1)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(onlypath)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	zstr_mgr result = cobj->getURI(onlypath);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getAcceptableContent)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	htab_mgr result = cobj->getAcceptableContent();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getHeaders)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	htab_mgr result = cobj->getHeaders();
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
	zstr_mgr result = cobj->getBestAccept();
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
	zstr_mgr result = cobj->getClientAddress(trustForward);
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
	zval_mgr result = cobj->getJsonRawBody(useArray);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getRawBody)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	zstr_mgr result = cobj->getRawBody();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getLanguages)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	htab_mgr result = cobj->getLanguages();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getUserAgent)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	zstr_mgr result = cobj->getUserAgent();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getBestCharset)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	zstr_mgr result = cobj->getBestCharset();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getClientCharsets)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	htab_mgr result = cobj->getClientCharsets();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getBestLanguage)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	zstr_mgr result = cobj->getBestLanguage();
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
	zstr_mgr result = cobj->getContentType();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getHttpHost)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	zstr_mgr result = cobj->getHttpHost();
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_RequestGlobals, post)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	zobj_user result = cobj->post();
	result.return_zv(return_value);

}

ZEND_METHOD(Wcc_RequestGlobals, query)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	zobj_user result = cobj->query();
	//showmem("getQuery result", result);
	result.return_zv(return_value);

}

ZEND_METHOD(Wcc_RequestGlobals, server)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	zobj_user result = cobj->server();
	//showmem("getQuery result", result);
	result.return_zv(return_value);

}

ZEND_METHOD(Wcc_RequestGlobals, getPost)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	htab_read result = cobj->getPost();
	result.return_zv(return_value);

}

ZEND_METHOD(Wcc_RequestGlobals, getQuery)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	htab_read result = cobj->getQuery();
	//showmem("getQuery result", result);
	result.return_zv(return_value);

}

ZEND_METHOD(Wcc_RequestGlobals, getServer)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	htab_read result = cobj->getServer();
	//showmem("getQuery result", result);
	result.return_zv(return_value);

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
	htab_mgr result = cobj->getUploadedFiles(only_success, namekeys);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getDigestAuth)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	htab_mgr result = cobj->getDigestAuth();
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_RequestGlobals, getHTTPReferer)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	zstr_mgr result = cobj->getHTTPReferer();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RequestGlobals, getServerAddress)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	RequestGlobals* cobj = zval_toc<RequestGlobals>(ZEND_THIS);
	zstr_mgr result = cobj->getServerAddress();
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

	return SUCCESS;
}

//request_globals.cpp
#endif