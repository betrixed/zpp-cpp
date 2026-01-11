#ifndef HTMLGEM_H
#define HTMLGEM_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcc {

	//using str_rc = str_rc;

	class HtmlGem : public base_d {
	protected:
		uint64_t id_add_;

		str_rc label_class_;
		str_rc text_class_;
		str_rc datetime_fmt_;
		str_rc date_fmt_;

		str_rc date_icon_; // html - img to display small date icon

		htab_rc   styles_; // styles keyed by name, each an array of text properties
		htab_rc   label_keys1_; 
		htab_rc   label_keys2_;
		htab_rc   select_keys_;

		/* const char* kName = "name";
		const char* kValue = "value";
		const char* kId = "id";
		const char* kLabelEnd = "</label>";
		*/

		void  outWrapDiv(str_buf& out, str_ptr cname);
		void  endWrapDiv(str_buf& out);
		htab_rc label_method(htab_rc ps, int& labeltype);
		void outAttr(str_buf& out, str_ptr name, str_ptr value);
		void ifKeyAttr(str_buf& out, str_ptr key, htab_ptr ps);
		htab_ptr getLabelKeys1();
		htab_ptr getSelectKeys();


	public:

		static base_obj_mgr<HtmlGem> omg;

		HtmlGem();
		virtual ~HtmlGem();

		virtual void debug_info(htab_rw hw);

	// php 

		void setLabelClass(str_ptr s);
		void setDateIcon(str_ptr s);
		void setTextClass(str_ptr s);
		void setDateTimeFmt(str_ptr s);
		void setDTCallback(zend_object* callback);

		void setStyle(str_ptr name, val_ptr map);

		// convert array into string
		
		str_rc button(val_ptr pset);
		str_rc checkbox(val_ptr pset);
		str_rc check_value(val_ptr pset);
		str_rc datetime(val_ptr pset);
		str_rc datetime_value(val_ptr pset);
		str_rc datetime_text(str_ptr dtvalue);
		str_rc email(val_ptr pset);
		str_rc figure(val_ptr pset);
		str_rc hidden(val_ptr pset);

		str_rc linkTo(val_ptr pset);
		str_rc money(val_ptr pset);
		str_rc multiline(val_ptr pset);
		str_rc password(val_ptr pset);
		str_rc phone(val_ptr pset);
		str_rc plaintext(val_ptr pset);
		str_rc number(val_ptr pset);
		str_rc output(val_ptr pset);
		str_rc radio(val_ptr pset);
		str_rc select(val_ptr pset);
		str_rc submit(val_ptr pset);
		str_rc text_value(val_ptr pset);

	// none-php
		str_rc generateTag(str_ptr tag, htab_ptr pset);
		str_rc getTag(htab_ptr ps, htab_rw ex, str_ptr tag);

		str_rc inputType(val_ptr pset, str_ptr itype);
		str_rc getStyle(str_ptr name);

		str_rc ensureIdValue(htab_rw ps);
		str_rc label_front(htab_ptr ps);
		str_rc in_label(htab_ptr ps);
		str_rc out_label(htab_ptr ps);

		str_rc select_list(val_ptr pset);
		str_rc xcheck(val_ptr pset);
		
		void image_tag(str_buf& ss, str_ptr path);
		void figcaption_tag(str_buf& ss, str_ptr text);

		VIRTUAL_ZOBJPTR

	}; // class




}; // namespace
#endif