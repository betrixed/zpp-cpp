#ifndef HTMLGEM_CPP
#define HTMLGEM_CPP

#ifndef HTMLGEM_H
#include "htmlgem.h"
#endif

#ifndef MONEY_FMT_H
#include "money_fmt.h"
#endif


#ifndef HTMLGET_ARGINFO_H
#define HTMLGET_ARGINFO_H
extern "C" {
	#include "stub/htmlgem_arginfo.h"
}
#endif


namespace wcc {

using namespace zpp;
	
base_obj_mgr<HtmlGem>  HtmlGem::omg;

using  Value = val_ptr;

enum LabelLocate {
	NO_LABEL,
	IN_LABEL,
	OUT_LABEL
};

class HTGInit : public state_init {
public:
	HTGInit() : state_init() {}

	str_intern date_icon;
	str_intern date_fmt;
	str_intern datetime_fmt;
	str_intern text_class;
	str_intern label_class;
	str_intern styles;

	str_intern date_fmt_d;
	str_intern datetime_fmt_d;

	str_intern label_d;
	str_intern text_d;
	str_intern figurekey;
	str_intern figcaption;
	str_intern idkey;
	str_intern namekey;
	str_intern format;

	str_intern classkey;
	str_intern blank;
	str_intern labelkey;
	str_intern content_key;
	str_intern endtag;
	str_intern buttonkey;
	str_intern divkey;
	str_intern tagendl;
	str_intern valuekey;
	str_intern textkey;
	str_intern checked;

	str_intern typekey;
	str_intern checkboxkey;
	str_intern inputtag;
	str_intern forkey;
	str_intern emptyset;
	str_intern checkmark;
	str_intern nbspace;
	str_intern submit;
	str_intern Submit;

	str_intern inlabel;
	str_intern number;
	str_intern radio;
	str_intern email;
	str_intern hidden;
	str_intern placehold;
	str_intern aria_describe;

	str_intern placehold_d;
	str_intern aria_describe_d;
	str_intern money;
	str_intern tel;
	str_intern password;
	str_intern xcheck;
	str_intern hrefkey;
	str_intern iconkey;
	str_intern glyphkey;
	str_intern hash_char;

	str_intern listkey;
	str_intern optionkey;
	str_intern begintag;
	str_intern selectkey;
	str_intern semicolon;
	str_intern imgkey;
	str_intern srckey;
	str_intern stylekey;
	str_intern caption_str;

	str_intern file_key;
	str_intern level_key;
	str_intern nearest_key;
	str_intern data_target;
	str_intern data_toggle;
	str_intern level_item;
	str_intern fmt_key;
	str_intern datetime_class;
	str_intern size_key;
	str_intern maxlength;


	str_intern textarea;

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

void HtmlGem::debug_info(htab_rw hw)
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
	date_icon_ = str_temp("@theme/calendar-50.png");

	id_add_ = 0;

	htab_rw hw(styles_);

	str_temp figure_style("float:left;width:47%;margin:10px;");
	hw.set(HTG.figurekey, figure_style);

	str_temp caption_style("border-style:solid; padding:4px;font-size:0.9em;");
	hw.set(HTG.figcaption, caption_style);
}


void HtmlGem::setLabelClass(str_ptr v)
{
	label_class_ = v;
}

void HtmlGem::setTextClass(str_ptr v)
{
	text_class_ = v;
}

void HtmlGem::setDateTimeFmt(str_ptr v)
{
	datetime_fmt_ = v;
}

void HtmlGem::setDateIcon(str_ptr v)
{
	date_icon_ = v;
}

str_rc 
HtmlGem::ensureIdValue(htab_rw ht)
{
	str_rc result;

	result = ht.get(HTG.idkey);

	if( result.ok() )
	{
		//zend_printf("has id\n");
		return result;
	}

	result = ht.get(HTG.namekey);

	str_buf nameval;

	if (result.ok())
	{
		nameval << str_ptr(result);
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


str_rc  
HtmlGem::output(val_ptr item)
{
	if (item.isString())
	{
		return item;
	}

	str_buf buf;
	//str_rc temp;

	if (item.isArray())
	{
		bool first = true;
		htab_walk wk;
		auto k = wk.key();
		auto v = wk.value();
		

		for(wk.start(item.zarray()); wk.ok(); wk.next())
		{
			if (first) {
				first = false;
			}
			else {
				buf << ',';
			}
			if (!k.isLong())
			{
				buf << '[' << k.zstr() << "]: ";
			}
			//temp = v.to_zstr();
			buf << v;
		}
	}
	else {
		//temp = item.to_zstr();
		buf << item;
	}
	return buf.zstr();
}

str_rc
HtmlGem::generateTag(str_ptr tag, htab_ptr pset)
{
	str_buf out;

	out << "<" << tag;
	htab_walk walk;
	//zend_printf("generateTag\n");
	auto arg = walk.key();
	auto val = walk.value();

	str_ptr zuse;

	for(walk.start(pset); walk.ok(); walk.next()) 
	{
		if (val.isObject())
		{
			obj_ptr obj(val.zobject());
			if (obj.isDateTime()) {
				val_rc inout(datetime_fmt_);

				inout = obj.call(HTG.format, inout);
				zuse = inout;
				if (zuse.size())
				{
					out << zuse;
				}
			}
		}
		str_rc ws;

		if (arg.isLong()) {
			out << " " << val.zstr();
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
	return out.zstr();
}

str_rc 
HtmlGem::getTag(htab_ptr ps, htab_rw ex, str_ptr tag)
{
	ex.merge(ps);
	val_ptr test = ex.get(HTG.classkey);
	
	//ex.show_data("merged ps");

	//showmem("get_tag", test);
	if (!test.isNull())
	{
		val_rc cdef(test);
		val_ptr cdef_use(cdef);

		val_rc cset = ex.get(HTG.classkey);
		val_ptr  cset_use(cset);

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

			htab_ptr cset_ht(cset);
			htab_ptr cdef_ht(cdef);

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

str_rc HtmlGem::label_front(htab_ptr ps)
{
	str_buf out;

	out << '<' << HTG.labelkey;

	str_ptr id = ps.get(HTG.idkey);

	if (id.ok()) 
	{
		out << " for=";
		out.quote_name(id);
	}
	str_ptr labclass = ps.get(HTG.classkey);

	if (labclass.isNull()) {
		labclass = label_class_;
	}

	if (labclass.size()) {
		out << " class=";
		out.quote_name(labclass);
	}
	out << '>';
	return out.zstr();
}

str_rc 
HtmlGem::in_label(htab_ptr ps)
{
	str_buf out;

	out << label_front(ps);

	Value label = ps.get(HTG.labelkey);
	Value content = ps.get(HTG.content_key);

	out << HTG.blank;
	if (label.isArray()) {
		htab_ptr hlab(label);
		label = hlab.get(HTG.labelkey);
	}
	out << label.zstr();
	str_ptr temp(content);
	if (temp.ok()) {
		out << HTG.blank << temp;
	}
	out << HTG.blank << HTG.endtag << HTG.labelkey << '>';
	return  out.zstr();
}; 

str_rc HtmlGem::out_label(htab_ptr  ps)
{
	str_buf out;
	//zend_printf("out_label\n");

	out << label_front(ps);

	Value label = ps.get(HTG.labelkey);
	Value content = ps.get(HTG.content_key);

	if (label.isArray()) {
		htab_ptr wl(label);
		label = wl.get(HTG.labelkey);
	}
	str_ptr temp(label);

	if (label.ok()) {
		out << temp;
	}

	out << /*blank <<*/ HTG.endtag << HTG.labelkey << '>';
	
	temp = content.zstr();
	if (temp.ok()) {
		out << /*blank << */ temp;
	}
	//showstr("exit out_label", out);
	return out.zstr();
}; 


str_rc 
HtmlGem::button(val_ptr pset)
{
	htab_rc pscopy(pset.zarray());

	htab_rw ps(pscopy);

	str_rc content = ps.get(HTG.content_key);

	if (content.ok()) {
		ps.unset(HTG.content_key);
	}

	str_buf out;

	out << generateTag(HTG.buttonkey, ps) << content << HTG.endtag << HTG.buttonkey << '>';
	return  out.zstr();
}


void HtmlGem::outWrapDiv(str_buf& out, str_ptr cname)
{
	out << '<' << HTG.divkey << ' ' << HTG.classkey << '=';
	out.quote_name(cname); 
	out << '>';
}

void HtmlGem::endWrapDiv(str_buf& out) {
	out << HTG.endtag << HTG.divkey << HTG.tagendl;
}

str_rc HtmlGem::checkbox(val_ptr pset)
{
	htab_rc pscopy(pset.zarray());

	htab_rw ps(pscopy);

	str_rc id = ensureIdValue(ps);

	str_buf out;

	str_rc wrapdiv = ps.get(HTG.divkey);

	if (wrapdiv.size()) {
		outWrapDiv(out, wrapdiv);
		ps.unset(HTG.divkey);
	}

	str_rc text = ps.get(HTG.labelkey);
	if (text.size()) {
		ps.unset(HTG.labelkey);
	}
	else {
		text = ps.get(HTG.textkey);
		if (text.size()) {
			ps.unset(HTG.textkey);
		}
	}
	
	val_ptr check = ps.get(HTG.checked);

	if (!check.isNull()) {
		int checkval = check.zlong();
		ps.unset(HTG.checked); 
		//check.init(); //  check now invalid
		if (checkval != 0) {
			str_rc cval = HTG.checked;
			//showstr("pushback", cval);
			ps.push_back(cval);
			//showdata("checkbox", ps);
		}
	}

	if (!ps.has_key(HTG.valuekey)) {
		ps.set(HTG.valuekey, 1);
	}

	htab_rc   htemp;

	htab_rw hp(htemp);

	hp.set(HTG.typekey, HTG.checkboxkey);

	str_rc tag = getTag(ps, hp, HTG.inputtag);
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
	return  out.zstr();
}


str_rc 
HtmlGem::check_value(val_ptr pset)
{
	str_buf out;
	htab_ptr ps(pset.zarray());

	str_ptr label = ps.get(HTG.labelkey);

	if (label.size()) {
		out << '<' << HTG.labelkey << '>' << label << HTG.endtag << label << '>';
	}

	str_ptr ck = ps.get(HTG.checked);
	if (ck.size()) {
		out << HTG.nbspace << HTG.checkmark << HTG.nbspace;
	}
	else {
		out << HTG.nbspace << HTG.emptyset << HTG.nbspace;
	}
	return  out.zstr();
}


str_rc HtmlGem::submit(val_ptr pset)
{
	htab_rc  tab_list;
	// defaults
	htab_rw list(tab_list);

	//defaults
	list.set(HTG.typekey,HTG.submit);
	list.set(HTG.valuekey, HTG.Submit);

	return getTag(htab_ptr(pset), list, HTG.inputtag);
}

str_rc HtmlGem::datetime_value(val_ptr pset)
{
	str_buf out;

	htab_rc pcopy(pset.zarray());
	htab_rw ps(pcopy);

	val_ptr val = ps.get(HTG.valuekey);
	str_ptr fmt = ps.get(HTG.format);
	
	val_rc vdate;

	if (fmt.size() && !val.isNull()) 
	{
		vdate = datetime_obj::date(fmt, val);	
	}
	else {
		vdate = val;
	}

	str_buf ds; //date string buffer
	ds << HTG.nbspace;
	if (!val.isNull()) {
	    ds << val.vstr();
	}
	else {
		ds << HTG.emptyset;
	}
	ds << HTG.nbspace;

	Value label = ps.get(HTG.labelkey);

	str_rc datetext = ds.zstr();

	if (!label.isNull()) {
		ps.set(HTG.content_key, datetext);
		out << out_label(ps);
	}
	else {
		out << datetext;
	}
	out << endl;

	return out.zstr();
}


/**
 * May show recent dates as in the past
 */
str_rc
HtmlGem::datetime_text(str_ptr dtvalue)
{
	str_rc dstr(dtvalue);

	datetime_obj before(dstr);
	datetime_obj now;

	dt_interval diff = now.diff(before);

	int years = diff.years();
	int months = diff.months();
	int days = diff.days();

	if ((years < 1) && (months < 1)) {
		str_buf buf;
		if (days >= 14) {
			buf << int(days / 7 ) << " weeks ago";
		}
		else {
			buf << days << " days ago";
		}
		return buf.zstr();
	}

	return before.format(date_fmt_);

}; // namespace


str_rc
HtmlGem::text_value(val_ptr pset)
{
	htab_rc pscopy(pset.zarray());
	htab_rw ps(pscopy);

	htab_rw keys2(label_keys2_);

	if (keys2.size() == 0) {
		keys2.push_back(HTG.labelkey);
		keys2.push_back(HTG.label_class);
	}

	str_buf out;

	htab_rc mylabel = htab_rc::extract(keys2,ps);
	out << out_label(mylabel);

	str_ptr text = ps.get(HTG.valuekey);

	if (text.size()) {
		out << HTG.nbspace << text << HTG.nbspace;
	}
	else {
		out << HTG.nbspace << HTG.emptyset << HTG.nbspace;
	}
	return out.zstr();
}

htab_ptr 
HtmlGem::getLabelKeys1() 
{
	htab_rw hw(label_keys1_);

	if (hw.size() == 0)
	{
		hw.push_back(HTG.idkey);
		hw.push_back(HTG.content_key);
	}
	return label_keys1_;
}

htab_rc
HtmlGem::label_method(htab_rw ps, int& labeltype)
{
	htab_ptr kist = getLabelKeys1();
	htab_rc  result = htab_rc::extract(kist, ps);
	htab_rw hw_label(result);

	//showdata("hw_label", hw_label);

	str_rc ltext = ps.get(HTG.labelkey);

	if (ltext.size()) {
		
		labeltype = LabelLocate::OUT_LABEL;

		hw_label.set(HTG.labelkey, ltext);
		ps.unset(HTG.labelkey);
	}
	else {
		//zend_printf("inlabel\n");
		//showarray("ps", ps);
		//showmem("ltext", ltext);

		ltext = ps.get(HTG.inlabel);

		//showmem("ltext", ltext);
		
		if (ltext.size()) {
			labeltype = LabelLocate::IN_LABEL;
			hw_label.set(HTG.labelkey, ltext);
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


str_rc
HtmlGem::inputType(val_ptr pset, str_ptr itype)
{
	htab_rc atype_ht;
	htab_rw atype(atype_ht);

	atype.set(HTG.typekey, itype);

	//atype.show_data("atype ");
	
	htab_rc pscopy(pset.zarray());
	htab_rw ps(pscopy);
	
	ensureIdValue(ps);

	//ps.show_data("psid -- 1 ");

	str_buf out;

	if (!ps.has_key(HTG.classkey)) {
		ps.set(HTG.classkey, text_class_);
		//ps.show_data("psid -- 2 ");
	}

	str_rc wrapdiv = ps.get(HTG.divkey);

	if (wrapdiv.size()) {
		outWrapDiv(out, wrapdiv);
		ps.unset(HTG.divkey);
	}
	
	int label_loc = LabelLocate::NO_LABEL;

	htab_rc ht_label = label_method(ps, label_loc);
	htab_rw ldata(ht_label);

	str_rc input =  getTag(ps, atype, HTG.inputtag);
	
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

	if (wrapdiv.size()) {
		endWrapDiv(out);
	}
	return out.zstr();

}

str_rc
HtmlGem::plaintext(val_ptr pset)
{
	return inputType(pset, HTG.textkey);
}

str_rc
HtmlGem::number(val_ptr pset)
{
	return inputType(pset, HTG.number);
}

str_rc
HtmlGem::radio(val_ptr pset)
{
	htab_rc pscopy(pset.zarray());

	htab_rw ps(pscopy);

	str_rc label_s = ps.get(HTG.labelkey);

	if (label_s.size()) {
		ps.unset(HTG.labelkey);
	}
	str_rc id = ensureIdValue(ps);

	str_buf out;

	val_rc altered(ps);
	out << inputType(altered, HTG.radio);

	if (label_s.size()) 
	{
		htab_rc ldata_ht;
		htab_rw ldata(ldata_ht);

		ldata.set(HTG.idkey, id);
		ldata.set(HTG.labelkey,label_s);

		out << HTG.nbspace << out_label(ldata);
	}
	return out.zstr();
}

str_rc
HtmlGem::email(val_ptr pset)
{
	htab_rc pscopy(pset.zarray());
	htab_rw ps(pscopy);

	str_rc test = ps.get(HTG.placehold);

	if (test.isNull()) {
		ps.set(HTG.placehold, HTG.placehold_d);
	}
	test = ps.get(HTG.aria_describe);
	if (test.isNull()) {
		ps.set(HTG.aria_describe,HTG.aria_describe_d);
	}
	val_rc altered(pscopy);

	return inputType(altered,HTG.email);
}

str_rc 
HtmlGem::hidden(val_ptr pset)
{
	return inputType(pset, HTG.hidden);
}

str_rc 
HtmlGem::money(val_ptr pset)
{
	return inputType(pset, HTG.money);
}

str_rc 
HtmlGem::phone(val_ptr pset)
{
	return inputType(pset, HTG.tel);
}

str_rc 
HtmlGem::password(val_ptr pset)
{
	return inputType(pset, HTG.password);
}

str_rc
HtmlGem::xcheck(val_ptr pset)
{
	htab_rc pscopy(pset.zarray());
	htab_rw ps(pscopy);

	if (ps.has_key(HTG.namekey)) {
		ps.set(HTG.namekey, HTG.xcheck);
	}

	htab_rc extra;
	htab_rw ex(extra);

	ex.set(HTG.typekey, HTG.hidden)                                                                                                                                                                                                               ;
	return getTag(ps, ex, HTG.inputtag);
}

void HtmlGem::outAttr(str_buf& out, str_ptr name, str_ptr value)
{
	out << HTG.blank <<  name <<  '=';
	out.quote_name(value);
}

static 
str_rc glyph_out(str_ptr glyph)
{
	str_buf ss;

	ss << "<span class=\"icon\"><i class=\"fas fa-";
	ss << glyph.vstr();
	ss << "\"></i></span>";

	return ss.zstr();
}

str_rc 
HtmlGem::linkTo(val_ptr pset)
{
	htab_rc   pscopy(pset.zarray());
	htab_rw ps(pscopy);
	
	str_rc href = ps.get(HTG.hrefkey);

	if (href.size()) {
		ps.unset(HTG.hrefkey);
	}
	else {
		href = HTG.hash_char;
	}

	str_rc icon = ps.get(HTG.iconkey);
	if (!icon.isNull()) {
		ps.unset(HTG.iconkey);
	}
	str_rc text = ps.get(HTG.textkey);

	if (text.size()) {
		ps.unset(HTG.textkey);
	}
	else {
		str_rc glyph = ps.get(HTG.glyphkey);

		if (glyph.size()) {
			ps.unset(HTG.glyphkey);
			text = glyph_out(glyph);
		}
		else {
			text = str_empty();
		}
	}
	str_buf out;

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

	if (icon.size()) {
		out << "<i";
		outAttr(out, HTG.classkey, icon);
		out << '>' << "</i>";
	}

	out << text << "</a>";
	return out.zstr();
}

void HtmlGem::ifKeyAttr(str_buf& out, str_ptr key, htab_ptr ps)
{
	str_ptr val = ps.get(key);
	if (val.ok()) {
		outAttr(out, key, val);
	}
}

htab_ptr HtmlGem::getSelectKeys()
{
	if (htab_ptr(select_keys_).size()==0)
	{
		htab_rw hw(select_keys_);

		hw.set(HTG.idkey,true);
		hw.set(HTG.classkey,true);
		hw.set(HTG.namekey,true);
		hw.set(HTG.listkey,true);
		hw.set(HTG.valuekey,true);
	}
	return select_keys_;
}

str_rc 
HtmlGem::select_list(val_ptr pset)
{
	str_buf out;
	val_ptr test;

	htab_rc   pscopy(pset.zarray());
	htab_rw ps(pscopy);

	val_rc list = ps.get(HTG.listkey);


	if (test.isArray()) {
		htab_ptr options(list);

		val_rc selected = ps.get(HTG.valuekey);

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
			htab_ptr select_keys = getSelectKeys();

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

			str_rc  selected_as_str;
			str_ptr suse;

			test = selected;

			if (!test.isNull())
			{
				selected_as_str = test.to_zstr();
				//showstr("value selected_str", selected_as_str);
			}
			suse = selected_as_str;

			for(op.start(options); op.ok(); op.next()) 
			{
				str_rc option_key = skey.to_zstr();

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
	return out.zstr();
}

str_rc
 HtmlGem::select(val_ptr pset)
{
	htab_rc pset_copy(pset.zarray());

	htab_rw ps(pset_copy);

	ensureIdValue(ps);
	str_buf out;

	str_rc wrapdiv = ps.get(HTG.divkey);

	if (wrapdiv.size()) {
		outWrapDiv(out,wrapdiv);
		ps.unset(HTG.divkey);
	}
	int method = 0;
	htab_rc label_ht = label_method(ps,method);
	htab_rw label(label_ht);

	str_rc select = select_list(pset);

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
	return out.zstr();
}

void HtmlGem::setStyle(str_ptr name, val_ptr value)
{
	htab_rw hw(styles_);

	hw.set(name,value); // map or string "xx:ss;"
}

htab_rc
style_toArray(str_ptr style)
{

	val_rc pairs_val = explode(HTG.semicolon, style, 0);
	htab_ptr pairs(pairs_val);

	htab_rc result;
	htab_rw hw(result);

	htab_walk wk;

	auto sval = wk.value();

	for(wk.start(pairs); wk.ok(); wk.next()) 
	{
		std::string_view sview = sval.vstr();

		auto ix = sview.find(':');

		if ((ix != std::string_view::npos) && (ix > 0)) 
		{
			str_rc key(sview.substr(0,ix));
			str_rc val(sview.substr(ix+1));

			hw.set(key, val);
		}
	}
	return result;
}

htab_rc 
mergeStyles(val_ptr list1, val_ptr list2)
{
	htab_rc result;


	if (list1.isString()) {
		result = style_toArray(list1.to_zstr());
	}
	else if (list1.isArray()) 
	{
		result = list1.zarray();
	}
	htab_rw hw(result);
	if (list2.isString())
	{
		htab_rc merge2 = style_toArray(list2.to_zstr());
		hw.merge(merge2);
	}
	else if (list2.isArray()) {
		htab_ptr ht2(list2);
		if (ht2.size())
			hw.merge(ht2);
	}
	return result;
}

str_rc 
array_toStyle(htab_ptr slist)
{
	str_buf ss;

	htab_walk wk;
	auto key = wk.key();
	auto value = wk.value();

	for(wk.start(slist); wk.ok(); wk.next())
	{
		ss << key.vstr() << ":" << value.vstr() << ";";
	}
	return ss.zstr();
}

str_rc
HtmlGem::getStyle(str_ptr skey)
{
	val_ptr style = htab_ptr(styles_).get(skey);
	if (style.isArray())
	{
		return array_toStyle(style);
	}
	return style;
}

void
HtmlGem::image_tag(str_buf& ss, str_ptr path)
{
	ss <<  HTG.begintag << HTG.imgkey;

	outAttr(ss, HTG.srckey, path);
	
	str_rc tagstyle  = getStyle(HTG.imgkey);

	if (!tagstyle.isNull()) {
		outAttr(ss, HTG.stylekey, tagstyle);
	}
	
	ss << HTG.tagendl;
}

void
HtmlGem::figcaption_tag(str_buf& ss, str_ptr text)
{
	ss << HTG.begintag << HTG.figcaption;

	str_rc tagstyle = getStyle(HTG.figcaption);

	if (!tagstyle.isNull()) {
		outAttr(ss, HTG.stylekey, tagstyle);
	}
	
	ss << ">" << text << HTG.endtag << HTG.figcaption << HTG.tagendl;
}

str_rc HtmlGem::figure(val_ptr pset)
{
	str_buf out;

	htab_rc pscopy(pset.zarray());
	htab_rw ps(pscopy);

	str_rc src = ps.get(HTG.file_key);
	str_ptr test(src);

	if (test.size()) {
		out << '<' << HTG.figurekey;

		val_rc s =  getStyle(HTG.figurekey);
		val_rc morestyle = ps.get(HTG.figurekey);

		htab_rc tagstyle = mergeStyles(s, morestyle);
		str_rc attr = array_toStyle(tagstyle);

		outAttr(out, HTG.stylekey , attr );
		out << HTG.tagendl; // end figure tag open
		image_tag(out, src);

		str_ptr caption = ps.get(HTG.caption_str);
		if (caption.size()) {
			figcaption_tag(out, caption);
		}
		out << HTG.endtag << HTG.figurekey << HTG.tagendl;
	}
	return out.zstr();
}


/* setup properties for datetime picker class */
str_rc 
HtmlGem::datetime(val_ptr pset)
{
	str_buf out;
	str_rc  temp;

	htab_rc pscopy(pset.zarray());
	//showarray("pscopy", pscopy);
	
	htab_rw ps(pscopy);

	str_rc idstr = ensureIdValue(ps);
	//showstr("idstr", idstr);

	str_buf dateid;

	dateid << "pick" << idstr;

	str_rc dateid_str = dateid.zstr();
	//showstr("dateid_str", dateid_str);


	out << '<' << HTG.divkey; // level 

	outAttr(out, HTG.classkey, HTG.level_key);

	outAttr(out, HTG.idkey, dateid_str);

	outAttr(out, HTG.data_target, HTG.nearest_key);
	out << HTG.tagendl;

	outWrapDiv(out, HTG.level_item);//1 div level-item

	int method = 0;
	htab_rc label_ht = label_method(ps, method);
	htab_rw label(label_ht);

	//showarray("label table", label);

	str_buf dtclass;

	str_rc dtm_fmt =  ps.get(HTG.fmt_key);
	str_ptr test(dtm_fmt);

	if (test.isNull()){
		dtclass << HTG.datetime_class;
	}
	else {
		dtclass << test;
	}
	dtclass << "picker";

	//zend_printf("dtclass\n");

	str_rc dt_class_str = dtclass.zstr();

	htab_rc attrlist_ht;
	htab_rw attrlist(attrlist_ht);

	attrlist.set(HTG.typekey, HTG.textkey);

	str_buf dtinput;

	dtinput << dt_class_str << "-input";

	str_rc value =  dtinput.zstr(); 
	//val_rc value = dtinput.zstr();

	//zend_printf("dtinput\n");
	attrlist.set(HTG.classkey,value);

	str_buf atarg;

	atarg << "#" << dateid_str;

	str_rc atarg_str = atarg.zstr();

	attrlist.set(HTG.data_target, atarg_str);

	str_temp s24("24");

	attrlist.set(HTG.size_key,s24);
	attrlist.set(HTG.maxlength,s24);


	str_rc input = getTag(ps, attrlist, HTG.inputtag);
	//zend_printf("dt 4\n");

	if (method == OUT_LABEL) {
		label.set(HTG.content_key, input);
		temp = in_label(label);
		out << temp;
	}
	else if (method == IN_LABEL) 
	{
		label.set(HTG.content_key, input);
		temp = in_label(label);
		out << temp;
	}
	else {
		out << input;
	}
	//zend_printf("dt 5\n");
	str_rc icon = ps.get(HTG.iconkey);
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
	return out.zstr();
}


str_rc
HtmlGem::multiline(val_ptr pset)
{
	str_buf out;
	htab_ptr temp(pset);
	//showarray("pset", temp);
	htab_rc pscopy(temp);
	//showarray("pscopy", pscopy);
	

	htab_rw ps(pscopy);

	ensureIdValue(ps);
	

	val_rc clist = ps.get(HTG.classkey);

	if (val_ptr(clist).isNull()) {
		clist = text_class_;
	}
	
	str_rc wrapdiv = ps.get(HTG.divkey);

	if (wrapdiv.size()) {
		outWrapDiv(out, wrapdiv);
		ps.unset(HTG.divkey);
	}

	str_rc value = ps.get(HTG.valuekey);
	if (value.size()) {
		ps.unset(HTG.valuekey);
	}


	int method;
	//showarray("ps", ps);
	htab_rc label = label_method(ps, method);


	str_buf input;

	input << generateTag(HTG.textarea, ps);
	input << value << HTG.endtag << HTG.textarea << '>';

	str_rc input_str = input.zstr();

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

	return out.zstr();
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

	str_rc result = cobj->button(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, checkbox)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->checkbox(pset);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_HtmlGem, datetime)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->datetime(pset);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_HtmlGem, datetime_text)
{
	zend_string* s;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(s)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->datetime_text(s);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_HtmlGem, email)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->email(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, figure)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->figure(pset);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_HtmlGem, hidden)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->hidden(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, linkto)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->linkTo(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, money)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->money(pset);
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

	str_rc result = cobj->multiline(pset);
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

	str_rc result = cobj->number(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, phone)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->phone(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, password)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->password(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, radio)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->radio(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, plaintext)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->plaintext(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, select)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->select(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, select_list)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->select_list(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, output)
{
	zval* val;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(val)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->output(val);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_HtmlGem, submit)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->submit(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, datetime_value)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->datetime_value(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, check_value)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->check_value(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, text_value)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->text_value(pset);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlGem, xcheck)
{
	zval* pset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(pset)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlGem>(ZEND_THIS);

	str_rc result = cobj->xcheck(pset);
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

	str_rc result = cobj->getStyle(name);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_HtmlGem, moneyFormat)
{
	zend_string* slang = nullptr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR_OR_NULL(slang)
	ZEND_PARSE_PARAMETERS_END();

	str_rc temp;

	if (!slang) {
		temp = str_temp("en_AU");
	}
	else {
		temp = slang;
	}

	obj_rc result(MoneyFmt::omg.new_zobj());
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