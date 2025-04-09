#ifndef HTMLGEM_CPP
#define HTMLGEM_CPP

#include "htmlgem.h"

#ifndef HTMLGET_ARGINFO_H
#define HTMLGET_ARGINFO_H
extern "C" {
	#include "stub/htmlgem_arginfo.h"
}
#endif

namespace wcc {

	base_obj_mgr<HtmlGem>  HtmlGem::omg;

#ifndef MONEY_FMT_H
#include "money_fmt.h"
#endif


using  Value = zval_user;

enum LabelLocate {
	NO_LABEL,
	IN_LABEL,
	OUT_LABEL
};

class HTGInit : public state_init {
public:
	HTGInit() : state_init() {}

	zstr_intern date_icon;
	zstr_intern date_fmt;
	zstr_intern datetime_fmt;
	zstr_intern text_class;
	zstr_intern label_class;
	zstr_intern styles;

	zstr_intern date_fmt_d;
	zstr_intern datetime_fmt_d;

	zstr_intern label_d;
	zstr_intern text_d;
	zstr_intern figurekey;
	zstr_intern figcaption;
	zstr_intern idkey;
	zstr_intern namekey;
	zstr_intern format;

	zstr_intern classkey;
	zstr_intern blank;
	zstr_intern labelkey;
	zstr_intern content_key;
	zstr_intern endtag;
	zstr_intern buttonkey;
	zstr_intern divkey;
	zstr_intern tagendl;
	zstr_intern valuekey;
	zstr_intern textkey;
	zstr_intern checked;

	zstr_intern typekey;
	zstr_intern checkboxkey;
	zstr_intern inputtag;
	zstr_intern forkey;
	zstr_intern emptyset;
	zstr_intern checkmark;
	zstr_intern nbspace;
	zstr_intern submit;
	zstr_intern Submit;

	zstr_intern inlabel;
	zstr_intern number;
	zstr_intern radio;
	zstr_intern email;
	zstr_intern hidden;
	zstr_intern placehold;
	zstr_intern aria_describe;

	zstr_intern placehold_d;
	zstr_intern aria_describe_d;
	zstr_intern money;
	zstr_intern tel;
	zstr_intern password;
	zstr_intern xcheck;
	zstr_intern hrefkey;
	zstr_intern iconkey;
	zstr_intern glyphkey;
	zstr_intern hash_char;

	zstr_intern listkey;
	zstr_intern optionkey;
	zstr_intern begintag;
	zstr_intern selectkey;
	zstr_intern semicolon;
	zstr_intern imgkey;
	zstr_intern srckey;
	zstr_intern stylekey;
	zstr_intern caption_str;

	zstr_intern file_key;
	zstr_intern level_key;
	zstr_intern nearest_key;
	zstr_intern data_target;
	zstr_intern data_toggle;
	zstr_intern level_item;
	zstr_intern fmt_key;
	zstr_intern datetime_class;
	zstr_intern size_key;
	zstr_intern maxlength;


	zstr_intern textarea;

	virtual void init()
	{
		date_icon = "date_icon";
		date_fmt = "date_fmt";
		datetime_fmt = "datetime_fmt";
		text_class = "text_class";
		label_class = "label_class";
		styles = "styles";
		date_icon = "label_class";
		date_fmt_d = "Y-m-d";
		datetime_fmt_d = "Y-m-d H:i:s";

		label_d = "label";
		text_d = "gemtext";
		figurekey = "figure";
		figcaption = "figcaption";
		idkey = "id";
		namekey = "name";
		format = "format";

		classkey = "class";
		blank = " ";
		labelkey = "label";
		content_key = "content";
		endtag = "</";
		buttonkey = "button";
		divkey = "div";
		tagendl = ">\n";
		valuekey = "value";
		textkey = "text";
		checked = "checked";

		typekey = "type";
		checkboxkey = "checkbox";
		inputtag = "input";
		forkey = "for";
		emptyset = "&#x2205;";
		checkmark = "&#x2713;";
		nbspace = "&nbsp;";
		submit = "submit";
		Submit = "Submit";

		inlabel = "in-label";
		number = "number";
		radio = "radio";
		email = "email";
		hidden = "hidden";
		placehold = "placehold";
		aria_describe = "aria_describedby";

		placehold_d = "your@email.domain";
		aria_describe_d = "email help"; 
		money = "money";
		tel = "tel";
		password = "password";
		xcheck = "xcheck";
		hrefkey = "href";
		iconkey = "icon";
		glyphkey = "btn-glyph";
		hash_char = "#";

		listkey = "list";
		optionkey = "option";
		begintag = "<";
		selectkey = "select";
		semicolon = ";";
		imgkey = "img";
		srckey = "src";
		stylekey = "style";
		caption_str = "caption";
		file_key = "file";

		level_key = "level";
		nearest_key = "nearest";
		data_target = "data-target";
		data_toggle = "data-toggle";
		level_item = "level-item";
		fmt_key = "fmt";
		datetime_class = "datetime";
		size_key = "size";
		maxlength = "maxlength";

		textarea = "textarea";
	}

};
HTGInit HTG;

HtmlGem::~HtmlGem()
{

}

void HtmlGem::debug_info(htab_write hw)
{
	hw.set(HTG.date_icon, date_icon_);
	hw.set(HTG.date_fmt, date_fmt_);
	hw.set(HTG.datetime_fmt, datetime_fmt_);
	hw.set(HTG.text_class, text_class_);
	hw.set(HTG.label_class, label_class_);
	hw.set(HTG.styles, styles_);
}

HtmlGem::HtmlGem() : base_d()
{
	label_class_ = HTG.label_d;
	text_class_ =  HTG.text_d;
	datetime_fmt_ = HTG.datetime_fmt_d;
	date_fmt_ = HTG.date_fmt_d;
	date_icon_ = zstr_temp("@theme/calendar-50.png");

	id_add_ = 0;

	htab_write hw(styles_);

	zstr_temp figure_style("float:left;width:47%;margin:10px;");
	hw.set(HTG.figurekey, figure_style);

	zstr_temp caption_style("border-style:solid; padding:4px;font-size:0.9em;");
	hw.set(HTG.figcaption, caption_style);
}


void HtmlGem::setLabelClass(zstr_user v)
{
	label_class_ = v;
}

void HtmlGem::setTextClass(zstr_user v)
{
	text_class_ = v;
}

void HtmlGem::setDateTimeFmt(zstr_user v)
{
	datetime_fmt_ = v;
}

void HtmlGem::setDateIcon(zstr_user v)
{
	date_icon_ = v;
}

zstr_mgr 
HtmlGem::ensureIdValue(htab_write ht)
{
	zstr_mgr result;

	result = ht.get(HTG.idkey);

	if( result.ok() )
	{
		//zend_printf("has id\n");
		return result;
	}

	result = ht.get(HTG.namekey);

	zstr_buffer nameval;

	if (result.ok())
	{
		nameval << zstr_user(result);
	}
	else {
		nameval << HTG.namekey;
	}	
	//request-wide sequence?
	id_add_++;

	nameval << (int) id_add_;

	//nameval.append((int)id_add_);

	result = nameval.zstr();
	//showstr("set id", result);
	ht.set(HTG.idkey, result);
	return result;
}


zstr_mgr  
HtmlGem::output(zval_user item)
{
	zstr_buffer buf;
	dump_info   di(buf);
	di.dump(item, 0);
	return zstr_mgr(std::move(buf));
}

zstr_mgr
HtmlGem::generateTag(zstr_user tag, htab_read pset)
{
	zstr_buffer out;

	out << "<" << tag;
	htab_walk walk;
	//zend_printf("generateTag\n");
	auto arg = walk.key();
	auto val = walk.value();

	zstr_user zuse;

	for(walk.start(pset); walk.ok(); walk.next()) 
	{
		if (val.isObject())
		{
			zobj_user obj(val.zobject());
			if (obj.isDateTime()) {
				zval_mgr inout(datetime_fmt_);

				inout = obj.call(HTG.format, inout);
				zuse = inout;
				if (zuse.size())
				{
					out << zuse;
				}
			}
		}
		zstr_mgr ws;

		if (arg.isLong()) {
			out << " " << arg.zlong();
		}
		else {
			if (arg.isArray() || val.isArray()) {
				continue;
			}
			out << ' ' << arg.vstr();

			if (val.isString() && (val.size() > 0))
			{
				// TODO htmlspecialchars
				preg rex("#\"#");

				ws = rex.replace("&quot;", val.zstr());

			}
			else { // ?isBool()
				//zend_printf("from to_zstr\n");
				//showmem("val. ", val);
				ws = val.to_zstr();
			}
			out << "=\"" << ws << "\"";
		}
	}
	out << ">";
	return std::move(out);
}

zstr_mgr 
HtmlGem::getTag(htab_read ps, htab_write ex, zstr_user tag)
{
	ex.merge(ps);
	zval_user test = ex.get(HTG.classkey);
	
	//ex.show_data("merged ps");

	//showmem("get_tag", test);
	if (!test.isNull())
	{
		zval_mgr cdef(test);
		zval_user cdef_use(cdef);

		zval_mgr cset = ex.get(HTG.classkey);
		zval_user  cset_use(cset);

		if (cset_use.isNull()) {
			ex.set(HTG.classkey, cdef);
		}
		else {
			zend_string* retstr = nullptr;

			if (cset_use.getStringData(&retstr)) {
				cset = explode(HTG.blank, retstr);
			}
			else {
				cset.new_array();
			}

			if (cdef_use.getStringData(&retstr)) {
				cdef = explode(HTG.blank, retstr);
			}
			else {
				cdef.new_array();
			}

			htab_read cset_ht(cset);
			htab_read cdef_ht(cdef);

			if ((cdef_ht.size() > 0)  && (cset_ht.size() > 0))
			{
				cset = union_values(cset_ht, cdef_ht);
			}
			else if (cdef_ht.size() > 0) {
				cset = cdef;
			}
			cset_ht = cset_use.zarray();

			cset = implode(HTG.blank, cset_ht);
			ex.set(HTG.classkey, cset);
		}
	}
	return HtmlGem::generateTag(tag, ex);
}

zstr_mgr HtmlGem::label_front(htab_read ps)
{
	zstr_buffer out;

	out << '<' << HTG.labelkey;

	zstr_user id = ps.get(HTG.idkey);

	if (id.ok()) 
	{
		out << " for=";
		out.quote_name(id);
	}
	zstr_user labclass = ps.get(HTG.classkey);

	if (labclass.isNull()) {
		labclass = label_class_;
	}

	if (labclass.size()) {
		out << " class=";
		out.quote_name(labclass);
	}
	out << '>';
	return std::move(out);
}

zstr_mgr 
HtmlGem::in_label(htab_read ps)
{
	zstr_buffer out;

	out << label_front(ps);

	Value label = ps.get(HTG.labelkey);
	Value content = ps.get(HTG.content_key);

	out << HTG.blank;
	if (label.isArray()) {
		htab_read hlab(label);
		label = hlab.get(HTG.labelkey);
	}
	out << label.zstr();
	zstr_user temp(content);
	if (temp.ok()) {
		out << HTG.blank << temp;
	}
	out << HTG.blank << HTG.endtag << HTG.labelkey << '>';
	return  std::move(out);
}; 

zstr_mgr HtmlGem::out_label(htab_read  ps)
{
	zstr_buffer out;
	//zend_printf("out_label\n");

	out << label_front(ps);

	Value label = ps.get(HTG.labelkey);
	Value content = ps.get(HTG.content_key);

	if (label.isArray()) {
		htab_read wl(label);
		label = wl.get(HTG.labelkey);
	}
	zstr_user temp(label);

	if (label.ok()) {
		out << temp;
	}

	out << /*blank <<*/ HTG.endtag << HTG.labelkey << '>';
	
	temp = content.zstr();
	if (temp.ok()) {
		out << /*blank << */ temp;
	}
	//showstr("exit out_label", out);
	return std::move(out);
}; 


zstr_mgr 
HtmlGem::button(zval_user pset)
{
	htab_mgr pscopy(pset.zarray());

	htab_write ps(pscopy);

	zstr_mgr content = ps.get(HTG.content_key);

	if (content.ok()) {
		ps.unset(HTG.content_key);
	}

	zstr_buffer out;

	out << generateTag(HTG.buttonkey, ps) << content << HTG.endtag << HTG.buttonkey << '>';
	return  std::move(out);
}


void HtmlGem::outWrapDiv(zstr_buffer& out, zstr_user cname)
{
	out << '<' << HTG.divkey << ' ' << HTG.classkey << '=';
	out.quote_name(cname); 
	out << '>';
}

void HtmlGem::endWrapDiv(zstr_buffer& out) {
	out << HTG.endtag << HTG.divkey << HTG.tagendl;
}

zstr_mgr HtmlGem::checkbox(zval_user pset)
{
	htab_mgr pscopy(pset.zarray());

	htab_write ps(pscopy);

	zstr_mgr id = ensureIdValue(ps);

	zstr_buffer out;

	zstr_mgr wrapdiv = ps.get(HTG.divkey);

	zstr_user test(wrapdiv);

	if (test.size()) {
		outWrapDiv(out, test);
		ps.unset(HTG.divkey);
	}

	zstr_mgr text = ps.get(HTG.labelkey);

	test = text;
	if (test.size()) {
		ps.unset(HTG.labelkey);
	}
	else {
		text = ps.get(HTG.textkey);
		test = text;

		if (test.size()) {
			ps.unset(HTG.textkey);
		}
	}
	
	zval_user check = ps.get(HTG.checked);

	if (!check.isNull()) {
		int checkval = check.zlong();
		ps.unset(HTG.checked); 
		//check.init(); //  check now invalid
		if (checkval != 0) {
			ps.push_back(HTG.checked);
		}
	}

	if (!ps.has_key(HTG.valuekey)) {
		ps.set(HTG.valuekey, 1);
	}

	htab_mgr   htemp;

	htab_write hp(htemp);

	hp.set(HTG.typekey,HTG.checkboxkey);

	zstr_mgr tag = getTag(ps, hp, HTG.inputtag);
	out << tag << '\n';

	if (!text.isNull()) {

		out << '<' << HTG.labelkey << HTG.blank << HTG.classkey  << '=';
		out.quote_name(HTG.checkboxkey); 
		out << HTG.blank << HTG.forkey << '=';
		out.quote_name(id);
		out << '>' << text << HTG.endtag << HTG.labelkey << HTG.tagendl;
	}

	if (!wrapdiv.isNull()) {
		endWrapDiv(out);
	}
	return  std::move(out);
}


zstr_mgr 
HtmlGem::check_value(zval_user pset)
{
	zstr_buffer out;
	htab_read ps(pset.zarray());

	zstr_user label = ps.get(HTG.labelkey);

	if (label.size()) {
		out << '<' << HTG.labelkey << '>' << label << HTG.endtag << label << '>';
	}

	zstr_user ck = ps.get(HTG.checked);
	if (ck.size()) {
		out << HTG.nbspace << HTG.checkmark << HTG.nbspace;
	}
	else {
		out << HTG.nbspace << HTG.emptyset << HTG.nbspace;
	}
	return  std::move(out);
}


zstr_mgr HtmlGem::submit(zval_user pset)
{
	htab_mgr  tab_list;
	// defaults
	htab_write list(tab_list);

	//defaults
	list.set(HTG.typekey,HTG.submit);
	list.set(HTG.valuekey, HTG.Submit);

	return getTag(htab_read(pset), list, HTG.inputtag);
}

zstr_mgr HtmlGem::datetime_value(zval_user pset)
{
	zstr_buffer out;

	htab_mgr pcopy(pset.zarray());
	htab_write ps(pcopy);

	zval_user val = ps.get(HTG.valuekey);
	zstr_user fmt = ps.get(HTG.format);
	
	zval_mgr vdate;

	if (fmt.size() && !val.isNull()) 
	{
		vdate = datetime_obj::date(fmt, val);	
	}
	else {
		vdate = val;
	}

	zstr_buffer ds; //date string buffer
	ds << HTG.nbspace;
	if (!val.isNull()) {
	    ds << val.vstr();
	}
	else {
		ds << HTG.emptyset;
	}
	ds << HTG.nbspace;

	Value label = ps.get(HTG.labelkey);

	zstr_mgr datetext(std::move(ds));

	if (!label.isNull()) {
		ps.set(HTG.content_key, datetext);
		out << out_label(ps);
	}
	else {
		out << datetext;
	}
	out << endl;

	return std::move(out);
}


/**
 * May show recent dates as in the past
 */
zstr_mgr
HtmlGem::datetime_text(zstr_user dtvalue)
{
	zstr_mgr dstr(dtvalue);

	datetime_obj before(dstr);
	datetime_obj now;

	diff_dt diff = now.diff(before);

	int years = diff.years();
	int months = diff.months();
	int days = diff.days();

	if ((years < 1) && (months < 1)) {
		zstr_buffer buf;
		if (days >= 14) {
			buf << int(days / 7 ) << " weeks ago";
		}
		else {
			buf << days << " days ago";
		}
		return std::move(buf);
	}

	return before.format(date_fmt_);

}; // namespace


zstr_mgr
HtmlGem::text_value(zval_user pset)
{
	htab_mgr pscopy(pset.zarray());
	htab_write ps(pscopy);

	htab_write keys2(label_keys2_);

	if (keys2.size() == 0) {
		keys2.push_back(HTG.labelkey);
		keys2.push_back(HTG.label_class);
	}

	zstr_buffer out;

	htab_mgr mylabel = ps.extract(keys2);
	out << out_label(mylabel);

	zstr_user text = ps.get(HTG.valuekey);

	if (text.size()) {
		out << HTG.nbspace << text << HTG.nbspace;
	}
	else {
		out << HTG.nbspace << HTG.emptyset << HTG.nbspace;
	}
	return std::move(out);
}

htab_read 
HtmlGem::getLabelKeys1() 
{
	htab_write hw(label_keys1_);

	if (hw.size() == 0)
	{
		hw.push_back(HTG.idkey);
		hw.push_back(HTG.content_key);
	}
	return label_keys1_;
}

htab_mgr
HtmlGem::label_method(htab_write ps, int& labeltype)
{
	htab_read kist = getLabelKeys1();
	htab_mgr  result = ps.extract(kist);
	htab_write hw_label(result);

	//showdata("hw_label", hw_label);

	zstr_mgr ltext = ps.get(HTG.labelkey);
	zstr_user label(ltext);

	if (label.size()) {
		
		labeltype = LabelLocate::OUT_LABEL;

		hw_label.set(HTG.labelkey, label);
		ps.unset(HTG.labelkey);
	}
	else {
		//zend_printf("inlabel\n");
		//showarray("ps", ps);
		//showmem("ltext", ltext);

		ltext = ps.get(HTG.inlabel);
		label = ltext;
		//showmem("ltext", ltext);
		
		if (label.size()) {
			labeltype = LabelLocate::IN_LABEL;
			hw_label.set(HTG.labelkey, label);
			ps.unset(HTG.inlabel);
		}
		else {
			//zend_printf("no label\n");	
			labeltype = LabelLocate::NO_LABEL;
		}
	}
	//showarray("label data final", ldata);
	return result;
}


zstr_mgr
HtmlGem::inputType(zval_user pset, zstr_user itype)
{
	htab_mgr atype_ht;
	htab_write atype(atype_ht);

	atype.set(HTG.typekey, itype);

	//atype.show_data("atype ");
	
	htab_mgr pscopy(pset.zarray());
	htab_write ps(pscopy);
	
	ensureIdValue(ps);

	//ps.show_data("psid -- 1 ");

	zstr_buffer out;

	if (!ps.has_key(HTG.classkey)) {
		ps.set(HTG.classkey, text_class_);
		//ps.show_data("psid -- 2 ");
	}

	zstr_mgr wrapdiv = ps.get(HTG.divkey);
	zstr_user wuse(wrapdiv);

	if (wuse.size()) {
		outWrapDiv(out, wuse);
		ps.unset(HTG.divkey);
	}
	
	int label_loc = LabelLocate::NO_LABEL;

	htab_mgr ht_label = label_method(ps, label_loc);
	htab_write ldata(ht_label);

	zstr_mgr input =  (ps, atype, HTG.inputtag);
	
	//showstr("input tag", input);
	if (label_loc == LabelLocate::IN_LABEL) {
		ldata.set(HTG.content_key, input);
		out << in_label(ldata);
	}
	else if (label_loc == LabelLocate::OUT_LABEL)
	 {
		ldata.set(HTG.content_key, input);
		out << out_label(ldata);
	}
	else {
		out << '\n' << input;
	}

	if (wuse.size()) {
		endWrapDiv(out);
	}
	return std::move(out);

}

zstr_mgr
HtmlGem::plaintext(zval_user pset)
{
	return inputType(pset, HTG.textkey);
}

zstr_mgr
HtmlGem::number(zval_user pset)
{
	return inputType(pset, HTG.number);
}

zstr_mgr
HtmlGem::radio(zval_user pset)
{
	htab_mgr pscopy(pset.zarray());

	htab_write ps(pscopy);

	zstr_mgr label_s = ps.get(HTG.labelkey);
	zstr_user label(label_s);

	if (label.size()) {
		ps.unset(HTG.labelkey);
	}
	zstr_mgr id = ensureIdValue(ps);

	zstr_buffer out;

	zval_mgr altered(ps);
	out << inputType(altered, HTG.radio);

	if (label.size()) 
	{
		htab_mgr ldata_ht;
		htab_write ldata(ldata_ht);

		ldata.set(HTG.idkey, id);
		ldata.set(HTG.labelkey,label);

		out << HTG.nbspace << out_label(ldata);
	}
	return std::move(out);
}

zstr_mgr
HtmlGem::email(zval_user pset)
{
	htab_mgr pscopy(pset.zarray());
	htab_write ps(pscopy);

	zstr_user test = ps.get(HTG.placehold);

	if (test.isNull()) {
		ps.set(HTG.placehold, HTG.placehold_d);
	}
	test = ps.get(HTG.aria_describe);
	if (test.isNull()) {
		ps.set(HTG.aria_describe,HTG.aria_describe_d);
	}
	zval_mgr altered(pscopy);

	return inputType(altered,HTG.email);
}

zstr_mgr 
HtmlGem::hidden(zval_user pset)
{
	return inputType(pset, HTG.hidden);
}

zstr_mgr 
HtmlGem::money(zval_user pset)
{
	return inputType(pset, HTG.money);
}

zstr_mgr 
HtmlGem::phone(zval_user pset)
{
	return inputType(pset, HTG.tel);
}

zstr_mgr 
HtmlGem::password(zval_user pset)
{
	return inputType(pset, HTG.password);
}

zstr_mgr
HtmlGem::xcheck(zval_user pset)
{
	htab_mgr pscopy(pset.zarray());
	htab_write ps(pscopy);

	if (ps.has_key(HTG.namekey)) {
		ps.set(HTG.namekey, HTG.xcheck);
	}

	htab_mgr extra;
	htab_write ex(extra);

	ex.set(HTG.typekey, HTG.hidden)                                                                                                                                                                                                               ;
	return getTag(ps, ex, HTG.inputtag);
}

void HtmlGem::outAttr(zstr_buffer& out, zstr_user name, zstr_user value)
{
	out << HTG.blank <<  name <<  '=';
	out.quote_name(value);
}

static 
zstr_mgr glyph_out(zstr_user glyph)
{
	zstr_buffer ss;

	ss << "<span class=\"icon\"><i class=\"fas fa-";
	ss << glyph.vstr();
	ss << "\"></i></span>";

	return std::move(ss);
}

zstr_mgr 
HtmlGem::linkTo(zval_user pset)
{
	htab_mgr pscopy(pset.zarray());
	htab_write ps(pscopy);
	
	zstr_mgr href = ps.get(HTG.hrefkey);
	zstr_user test(href);

	if (test.size()) {
		ps.unset(HTG.hrefkey);
	}
	else {
		href = HTG.hash_char;
	}

	zstr_mgr icon = ps.get(HTG.iconkey);
	if (!icon.isNull()) {
		ps.unset(HTG.iconkey);
	}
	zstr_mgr text = ps.get(HTG.textkey);
	test = text;
	if (test.size()) {
		ps.unset(HTG.textkey);
	}
	else {
		zstr_mgr glyph = ps.get(HTG.glyphkey);
		test = glyph;
		if (test.size()) {
			ps.unset(HTG.glyphkey);
			text = glyph_out(test);
		}
		else {
			text = zstr_empty();
		}
	}
	zstr_buffer out;

	out << "<a ";

	outAttr(out, HTG.hrefkey, href);

	htab_walk walk;
	auto wkey = walk.key();
	auto wval = walk.value();

	//showdata("ps", ps);

	for(walk.start(ps); walk.ok(); walk.next()) 
	{
		outAttr(out, wkey, wval);
	}
	out << '>';
	test = icon;
	if (test.size()) {
		out << "<i";
		outAttr(out, HTG.classkey, test);
		out << '>' << "</i>";
	}

	out << text << "</a>";
	return std::move(out);
}

void HtmlGem::ifKeyAttr(zstr_buffer& out, zstr_user key, htab_read ps)
{
	zstr_user val = ps.get(key);
	if (val.ok()) {
		outAttr(out, key, val);
	}
}

htab_read HtmlGem::getSelectKeys()
{
	if (htab_read(select_keys_).size()==0)
	{
		htab_write hw(select_keys_);

		hw.set(HTG.idkey,true);
		hw.set(HTG.classkey,true);
		hw.set(HTG.namekey,true);
		hw.set(HTG.listkey,true);
		hw.set(HTG.valuekey,true);
	}
	return select_keys_;
}

zstr_mgr 
HtmlGem::select_list(zval_user pset)
{
	zstr_buffer out;
	zval_user test;

	htab_mgr   pscopy(pset.zarray());
	htab_write ps(pscopy);

	zval_mgr list = ps.get(HTG.listkey);
	test = list;

	if (test.isArray()) {
		htab_read options(list);

		zval_mgr selected = ps.get(HTG.valuekey);

		ps.unset(HTG.listkey);

		test = selected;

		if (!test.isNull()) {
			ps.unset(HTG.valuekey);
		}
		else {
			selected = (zend_long)0;
		}

		out << "<select";
		ifKeyAttr(out, HTG.idkey, ps);
		ifKeyAttr(out, HTG.classkey, ps);
		ifKeyAttr(out, HTG.namekey, ps);
		{
			htab_read select_keys = getSelectKeys();

			// any other attributes?
			htab_walk wk;
			auto attrkey = wk.key();
			auto attrval = wk.value();
			for (wk.start(ps); wk.ok(); wk.next())
			{
				test = select_keys.get(attrkey);

				if (test.isNull()) {
					outAttr(out, attrkey.zstr(), attrval.zstr());
				}
			}
			out << HTG.tagendl;
		}

		{
			htab_walk op;
			auto  skey = op.key();
			auto  sval = op.value();
			bool  selected_option = false;
			zstr_mgr  selected_as_str;
			zstr_user suse;

			test = selected;

			if (!test.isNull())
			{
				selected_as_str = test.to_zstr();
				//showstr("value selected_str", selected_as_str);
			}
			suse = selected_as_str;

			for(op.start(options); op.ok(); op.next()) 
			{
				zstr_mgr option_key = skey.to_zstr();

				if (suse.isNull())
				{
					selected_as_str = option_key;
					suse = selected_as_str;
				}
				out << HTG.begintag << HTG.optionkey;
				outAttr(out, HTG.valuekey, option_key);

				if (!zs_cmp(suse, option_key)) {
					out << " selected>";
				}
				else {
					out << '>';
				}
				out << sval.to_zstr() << HTG.endtag << HTG.optionkey <<  HTG.tagendl;
			}
			out <<  HTG.endtag <<  HTG.selectkey <<  HTG.tagendl;
			
		}
	}
	return std::move(out);
}

zstr_mgr
 HtmlGem::select(zval_user pset)
{
	htab_mgr pset_copy(pset.zarray());

	htab_write ps(pset_copy);

	ensureIdValue(ps);
	zstr_buffer out;

	zstr_mgr wrapdiv = ps.get(HTG.divkey);
	zstr_user test(wrapdiv);

	if (test.size()) {
		outWrapDiv(out,test);
		ps.unset(HTG.divkey);
	}
	int method = 0;
	htab_mgr label_ht = label_method(ps,method);
	htab_write label(label_ht);

	zstr_mgr select = select_list(pset);

	if (method == OUT_LABEL) {
		label.set(HTG.content_key,select);
		out << out_label(label);
	}
	else if (method == IN_LABEL) {
		label.set(HTG.content_key,select);
		out << in_label(label);
	}
	else {
		out << select << '\n';
	}
	return std::move(out);
}

void HtmlGem::setStyle(zstr_user name, zval_user value)
{
	htab_write hw(styles_);

	hw.set(name,value); // map or string "xx:ss;"
}

htab_mgr
style_toArray(zstr_user style)
{

	zval_mgr pairs_val = explode(HTG.semicolon, style, 0);
	htab_read pairs(pairs_val);

	htab_mgr result;
	htab_write hw(result);

	htab_walk wk;

	auto sval = wk.value();
	auto kval = wk.key();
	for(wk.start(pairs); wk.ok(); wk.next()) 
	{
		std::string_view sview = sval.vstr();

		auto ix = sview.find(':');

		if ((ix != std::string_view::npos) && (ix > 0)) 
		{
			zstr_mgr key(sview.substr(0,ix));
			zstr_mgr val(sview.substr(ix+1));

			hw.set(key, val);
		}
	}
	return result;
}

htab_mgr 
mergeStyles(zval_user list1, zval_user list2)
{
	htab_mgr result;


	if (list1.isString()) {
		result = style_toArray(list1.to_zstr());
	}
	else if (list1.isArray()) 
	{
		result = list1.zarray();
	}
	htab_write hw(result);
	if (list2.isString())
	{
		htab_mgr merge2 = style_toArray(list2.to_zstr());
		hw.merge(merge2);
	}
	else if (list2.isArray()) {
		htab_read ht2(list2);
		if (ht2.size())
			hw.merge(ht2);
	}
	return std::move(result);
}

zstr_mgr 
array_toStyle(htab_read slist)
{
	zstr_buffer ss;

	htab_walk wk;
	auto key = wk.key();
	auto value = wk.value();

	for(wk.start(slist); wk.ok(); wk.next())
	{
		ss << key.vstr() << ":" << value.vstr() << ";";
	}
	return std::move(ss);
}

zstr_mgr
HtmlGem::getStyle(zstr_user skey)
{
	zval_user style = htab_read(styles_).get(skey);
	if (style.isArray())
	{
		return array_toStyle(style);
	}
	return style;
}

void
HtmlGem::image_tag(zstr_buffer& ss, zstr_user path)
{
	ss <<  HTG.begintag << HTG.imgkey;

	outAttr(ss, HTG.srckey, path);
	
	zstr_mgr tagstyle  = getStyle(HTG.imgkey);

	if (!tagstyle.isNull()) {
		outAttr(ss, HTG.stylekey, tagstyle);
	}
	
	ss << HTG.tagendl;
}

void
HtmlGem::figcaption_tag(zstr_buffer& ss, zstr_user text)
{
	ss << HTG.begintag << HTG.figcaption;

	zstr_mgr tagstyle = getStyle(HTG.figcaption);

	if (!tagstyle.isNull()) {
		outAttr(ss, HTG.stylekey, tagstyle);
	}
	
	ss << ">" << text << HTG.endtag << HTG.figcaption << HTG.tagendl;
}

zstr_mgr HtmlGem::figure(zval_user pset)
{
	zstr_buffer out;

	htab_mgr pscopy(pset.zarray());
	htab_write ps(pscopy);

	zstr_mgr src = ps.get(HTG.file_key);
	zstr_user test(src);

	if (test.size()) {
		out << '<' << HTG.figurekey;

		zval_mgr s =  getStyle(HTG.figurekey);
		zval_mgr morestyle = ps.get(HTG.figurekey);

		htab_mgr tagstyle = mergeStyles(s, morestyle);
		zstr_mgr attr = array_toStyle(tagstyle);

		outAttr(out, HTG.stylekey , attr );
		out << HTG.tagendl; // end figure tag open
		image_tag(out, src);

		zstr_user caption = ps.get(HTG.caption_str);
		if (caption.size()) {
			figcaption_tag(out, caption);
		}
		out << HTG.endtag << HTG.figurekey << HTG.tagendl;
	}
	return std::move(out);
}


/* setup properties for datetime picker class */
zstr_mgr 
HtmlGem::datetime(zval_user pset)
{
	zstr_buffer out;

	htab_mgr pscopy(pset.zarray());
	htab_write ps(pscopy);

	zstr_mgr idstr = ensureIdValue(ps);

	zstr_buffer dateid;

	dateid << "pick" << idstr;

	zstr_mgr dateid_str(std::move(dateid));

	out << '<' << HTG.divkey; // level 

	outAttr(out, HTG.classkey, HTG.level_key);

	outAttr(out, HTG.idkey, dateid_str);

	outAttr(out, HTG.data_target, HTG.nearest_key);
	out << HTG.tagendl;

	outWrapDiv(out, HTG.level_item);//1 div level-item

	int method = 0;
	htab_mgr label_ht = label_method(ps, method);
	htab_write label(label_ht);

	//showarray("label table", label);

	zstr_buffer dtclass;

	zstr_mgr dtm_fmt =  ps.get(HTG.fmt_key);
	zstr_user test(dtm_fmt);

	if (test.isNull()){
		dtclass << HTG.datetime_class;
	}
	else {
		dtclass << test;
	}
	dtclass << "picker";

	//zend_printf("dtclass\n");

	zstr_mgr dt_class_str(std::move(dtclass));

	htab_mgr attrlist_ht;
	htab_write attrlist(attrlist_ht);

	attrlist.set(HTG.typekey, HTG.textkey);

	zstr_buffer dtinput;

	dtinput << dt_class_str << "-input";

	zval_mgr value(std::move(dtinput));

	//zend_printf("dtinput\n");
	attrlist.set(HTG.classkey,value);

	zstr_buffer atarg;

	atarg << "#" << dateid_str;

	zstr_mgr atarg_str(std::move(atarg));

	attrlist.set(HTG.data_target, atarg_str);

	zstr_temp s24("24");

	attrlist.set(HTG.size_key,s24);
	attrlist.set(HTG.maxlength,s24);


	zstr_mgr input = getTag(ps, attrlist, HTG.inputtag);
	//zend_printf("dt 4\n");

	if (method == OUT_LABEL) {
		label.set(HTG.content_key, input);
		out << out_label(label);
	}
	else if (method == IN_LABEL) 
	{
		label.set(HTG.content_key, input);
		out << in_label(label);
	}
	else {
		out << input;
	}
	//zend_printf("dt 5\n");
	zstr_mgr icon = ps.get(HTG.iconkey);
	if (!icon.isNull()) {
		ps.unset(HTG.iconkey);
	}
	else {
		icon = date_icon_;
	}
	//zend_printf("dt 6\n");
	endWrapDiv(out); // end 2 level-item
	outWrapDiv(out, HTG.level_item); // div 2 level-item
	out << '<' << HTG.divkey;  // div 3
	outAttr(out, HTG.data_target , atarg_str);

	outAttr(out, HTG.data_toggle, dt_class_str);
	out << HTG.tagendl;

	if (!icon.isNull()) {
		 image_tag(out, icon);
	}
	//zend_printf("dt 7\n");
	endWrapDiv(out); // end 2
	endWrapDiv(out); // end level-item
	endWrapDiv(out); // end level
	return std::move(out);
}


zstr_mgr
HtmlGem::multiline(zval_user pset)
{
	zstr_buffer out;
	htab_read temp(pset);
	//showarray("pset", temp);
	htab_mgr pscopy(temp);
	//showarray("pscopy", pscopy);
	

	htab_write ps(pscopy);

	ensureIdValue(ps);
	

	zval_mgr clist = ps.get(HTG.classkey);

	if (zval_user(clist).isNull()) {
		clist = text_class_;
	}
	
	zstr_mgr wrapdiv = ps.get(HTG.divkey);
	zstr_user test(wrapdiv);

	if (test.size()) {
		outWrapDiv(out, test);
		ps.unset(HTG.divkey);
	}

	zstr_mgr value = ps.get(HTG.valuekey);
	test = value;
	if (test.size()) {
		ps.unset(HTG.valuekey);
	}


	int method;
	//showarray("ps", ps);
	htab_mgr label = label_method(ps, method);


	zstr_buffer input;

	input << generateTag(HTG.textarea, ps);
	input << value << HTG.endtag << HTG.textarea << '>';

	zstr_mgr input_str(std::move(input));

	//showmem("input_str", input_str);

	//zend_printf("method = %d\n", method);

	if (method == OUT_LABEL || method == IN_LABEL) {
		ps.set(HTG.content_key, input_str);
		ps.set(HTG.labelkey, label);
		if (method==OUT_LABEL) {
			out << out_label(ps);
		}
		else {
			out << in_label(ps);
		}
	}
	else {
		out << input_str;
	}
	if (!wrapdiv.isNull())
	{
		out << HTG.endtag << HTG.divkey << HTG.tagendl;
	}

	return std::move(out);
}
}; //namespace wcc;
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&@@@@@@@@@@@@@@@@@@@@&&&&&&&&&&&&&&&&&&&&&

ZEND_METHOD(Wcc_HtmlGem, button)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->button(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, checkbox)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->checkbox(pset);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_HtmlGem, datetime)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->datetime(pset);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_HtmlGem, datetime_text)
{
	zend_string* s;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(s)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->datetime_text(s);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_HtmlGem, email)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->email(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, figure)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->figure(pset);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_HtmlGem, hidden)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->hidden(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, linkto)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->linkTo(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, money)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->money(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, multiline)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	//showmem("parameter array", pset);
	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->multiline(pset);
	//showmem("result - ", result);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_HtmlGem, number)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->number(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, phone)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->phone(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, password)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->password(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, radio)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->radio(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, plaintext)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->plaintext(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, select)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->select(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, select_list)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->select_list(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, output)
{
	zval* val;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(val)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->output(val);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_HtmlGem, submit)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->submit(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, datetime_value)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->datetime_value(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, check_value)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->check_value(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, text_value)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->text_value(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, xcheck)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->xcheck(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, setLabelClass)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);
	cobj->setLabelClass(cname);
}

ZEND_METHOD(Wcc_HtmlGem, setTextClass)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);
	cobj->setTextClass(cname);
}

ZEND_METHOD(Wcc_HtmlGem, setDateIcon)
{
	zend_string* html;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(html)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);
	cobj->setDateIcon(html);
}

ZEND_METHOD(Wcc_HtmlGem, setStyle)
{
	zend_string* name;
	zval* data;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(name)
	Z_PARAM_ZVAL(data)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);
	cobj->setStyle(name, data);
}

ZEND_METHOD(Wcc_HtmlGem, getStyle)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	zstr_mgr result = cobj->getStyle(name);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_HtmlGem, moneyFormat)
{
	zend_string* slang = nullptr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR_OR_NULL(slang)
	ZEND_PARSE_PARAMETERS_END();

	zstr_mgr temp;

	if (!slang) {
		temp = zstr_temp("en_AU");
	}
	else {
		temp = slang;
	}

	zobj_mgr result(MoneyFmt::omg.new_zobj());
	MoneyFmt* mf = zobj_toc<MoneyFmt>(result);
	mf->construct(temp);

	result.move_zv(return_value);
}

PHP_MINIT_FUNCTION(Wcc_HtmlGem_reg)
{
	auto ce = register_class_Wcc_HtmlGem();

	HtmlGem::omg.classEntry(ce);

	return SUCCESS;
}

//HTMLGEM_CPP
#endif