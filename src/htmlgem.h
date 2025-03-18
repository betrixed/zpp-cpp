#ifndef HTMLGEM_H
#define HTMLGEM_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcc {

	//using zstr_mgr = zstr_mgr;

	class HtmlGem : public base_d {
	protected:
		uint64_t id_add_;

		zstr_mgr label_class_;
		zstr_mgr text_class_;
		zstr_mgr datetime_fmt_;
		zstr_mgr date_fmt_;

		zstr_mgr date_icon_; // html - img to display small date icon

		htab_mgr   styles_; // styles keyed by name, each an array of text properties
		htab_mgr   label_keys1_; 
		htab_mgr   label_keys2_;
		htab_mgr   select_keys_;

		/* const char* kName = "name";
		const char* kValue = "value";
		const char* kId = "id";
		const char* kLabelEnd = "</label>";
		*/

		void  outWrapDiv(zstr_buffer& out, zstr_user cname);
		void  endWrapDiv(zstr_buffer& out);
		htab_mgr label_method(htab_write ps, int& labeltype);
		void outAttr(zstr_buffer& out, zstr_user name, zstr_user value);
		void ifKeyAttr(zstr_buffer& out, zstr_user key, htab_read ps);
		htab_read getLabelKeys1();
		htab_read getSelectKeys();


	public:

		static base_obj_mgr<HtmlGem> omg;

		HtmlGem();
		virtual ~HtmlGem();

		virtual void debug_info(htab_write hw);

	// php 

		void setLabelClass(zstr_user s);
		void setDateIcon(zstr_user s);
		void setTextClass(zstr_user s);
		void setDateTimeFmt(zstr_user s);
		void setDTCallback(zend_object* callback);

		void setStyle(zstr_user name, zval_user map);

		// convert array into string
		
		zstr_mgr button(zval_user pset);
		zstr_mgr checkbox(zval_user pset);
		zstr_mgr check_value(zval_user pset);
		zstr_mgr datetime(zval_user pset);
		zstr_mgr datetime_value(zval_user pset);
		zstr_mgr datetime_text(zstr_user dtvalue);
		zstr_mgr email(zval_user pset);
		zstr_mgr figure(zval_user pset);
		zstr_mgr hidden(zval_user pset);

		zstr_mgr linkTo(zval_user pset);
		zstr_mgr money(zval_user pset);
		zstr_mgr multiline(zval_user pset);
		zstr_mgr password(zval_user pset);
		zstr_mgr phone(zval_user pset);
		zstr_mgr plaintext(zval_user pset);
		zstr_mgr number(zval_user pset);
		zstr_mgr output(zval_user pset);
		zstr_mgr radio(zval_user pset);
		zstr_mgr select(zval_user pset);
		zstr_mgr submit(zval_user pset);
		zstr_mgr text_value(zval_user pset);

	// none-php
		zstr_mgr generateTag(zstr_user tag, htab_read pset);
		zstr_mgr getTag(htab_read ps, htab_write ex, zstr_user tag);

		zstr_mgr inputType(zval_user pset, zstr_user itype);
		zstr_mgr getStyle(zstr_user name);

		zstr_mgr ensureIdValue(htab_write ps);
		zstr_mgr label_front(htab_read ps);
		zstr_mgr in_label(htab_read ps);
		zstr_mgr out_label(htab_read ps);

		zstr_mgr select_list(zval_user pset);
		zstr_mgr xcheck(zval_user pset);
		
		void image_tag(zstr_buffer& ss, zstr_user path);
		void figcaption_tag(zstr_buffer& ss, zstr_user text);

#ifndef BASE_ZOBJPTR
		VIRTUAL_ZOBJPTR
#endif
	}; // class




}; // namespace
#endif