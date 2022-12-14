/*
 * This file was generated automatically by xsubpp version 1.9508 from the
 * contents of tmp. Do not edit this file, edit tmp instead.
 *
 *		ANY CHANGES MADE HERE WILL BE LOST!
 *
 */


/*  EQEMu:  Everquest Server Emulator
	Copyright (C) 2001-2004  EQEMu Development Team (http://eqemulator.net)

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY except by those people which sell it, which
		are required to give you total support for your newly bought product;
		without even the implied warranty of MERCHANTABILITY or FITNESS FOR
		A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "features.h"
#ifdef EMBPERL_XS_CLASSES
#include "../common/debug.h"
#include "embperl.h"

#include "object.h"

#ifdef THIS	 /* this macro seems to leak out on some systems */
#undef THIS		
#endif

XS(XS_Object_IsGroundSpawn); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_IsGroundSpawn)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Object::IsGroundSpawn(THIS)");
	{
		Object *		THIS;
		bool		RETVAL;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		RETVAL = THIS->IsGroundSpawn();
		ST(0) = boolSV(RETVAL);
		sv_2mortal(ST(0));
	}
	XSRETURN(1);
}



XS(XS_Object_Close); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_Close)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Object::Close(THIS)");
	{
		Object *		THIS;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		THIS->Close();
	}
	XSRETURN_EMPTY;
}


XS(XS_Object_Delete); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_Delete)
{
	dXSARGS;
	if (items < 1 || items > 2)
		Perl_croak(aTHX_ "Usage: Object::Delete(THIS, reset_state=false)");
	{
		Object *		THIS;
		bool		reset_state;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		if (items < 2)
			reset_state = false;
		else {
			reset_state = (bool)SvTRUE(ST(1));
		}

		THIS->Delete(reset_state);
	}
	XSRETURN_EMPTY;
}
XS(XS_Object_StartDecay); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_StartDecay)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Object::StartDecay(THIS)");
	{
		Object *		THIS;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		THIS->StartDecay();
	}
	XSRETURN_EMPTY;
}


XS(XS_Object_DeleteItem); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_DeleteItem)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: Object::DeleteItem(THIS, index)");
	{
		Object *		THIS;
		uint8		index = (uint8)SvUV(ST(1));

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		THIS->DeleteItem(index);
	}
	XSRETURN_EMPTY;
}

XS(XS_Object_IsObject); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_IsObject)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Object::IsObject(THIS)");
	{
		Object *		THIS;
		bool		RETVAL;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		RETVAL = THIS->IsObject();
		ST(0) = boolSV(RETVAL);
		sv_2mortal(ST(0));
	}
	XSRETURN(1);
}


XS(XS_Object_Save); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_Save)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Object::Save(THIS)");
	{
		Object *		THIS;
		bool		RETVAL;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		RETVAL = THIS->Save();
		ST(0) = boolSV(RETVAL);
		sv_2mortal(ST(0));
	}
	XSRETURN(1);
}


XS(XS_Object_SetID); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_SetID)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: Object::SetID(THIS, set_id)");
	{
		Object *		THIS;
		int16		set_id = (int16)SvUV(ST(1));

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		THIS->SetID(set_id);
	}
	XSRETURN_EMPTY;
}


XS(XS_Object_ClearUser); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_ClearUser)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Object::ClearUser(THIS)");
	{
		Object *		THIS;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		THIS->ClearUser();
	}
	XSRETURN_EMPTY;
}


XS(XS_Object_GetDBID); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_GetDBID)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Object::GetDBID(THIS)");
	{
		Object *		THIS;
		int32		RETVAL;
		dXSTARG;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		RETVAL = THIS->GetDBID();
		XSprePUSH; PUSHu((UV)RETVAL);
	}
	XSRETURN(1);
}

XS(XS_Object_GetID); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_GetID)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Object::GetID(THIS)");
	{
		Object *		THIS;
		int16		RETVAL;
		dXSTARG;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		RETVAL = THIS->GetID();
		XSprePUSH; PUSHu((UV)RETVAL);
	}
	XSRETURN(1);
}

XS(XS_Object_GetX); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_GetX)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Object::GetX(THIS)");
	{
		Object *		THIS;
		float		RETVAL;
		dXSTARG;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		RETVAL = THIS->GetX();
		XSprePUSH; PUSHn((double)RETVAL);
	}
	XSRETURN(1);
}

XS(XS_Object_GetY); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_GetY)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Object::GetY(THIS)");
	{
		Object *		THIS;
		float		RETVAL;
		dXSTARG;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		RETVAL = THIS->GetY();
		XSprePUSH; PUSHn((double)RETVAL);
	}
	XSRETURN(1);
}

XS(XS_Object_GetZ); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_GetZ)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Object::GetZ(THIS)");
	{
		Object *		THIS;
		float		RETVAL;
		dXSTARG;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		RETVAL = THIS->GetZ();
		XSprePUSH; PUSHn((double)RETVAL);
	}
	XSRETURN(1);
}

XS(XS_Object_GetHeading); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_GetHeading)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Object::GetHeading(THIS)");
	{
		Object *		THIS;
		float		RETVAL;
		dXSTARG;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		RETVAL = THIS->GetHeadingData();
		XSprePUSH; PUSHn((double)RETVAL);
	}
	XSRETURN(1);
}

XS(XS_Object_VarSave); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_VarSave)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Object::VarSave(THIS)");
	{
		Object *		THIS;
		int32		RETVAL;
		dXSTARG;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		RETVAL = THIS->VarSave();
		XSprePUSH; PUSHu((UV)RETVAL);
	}
	XSRETURN(1);
}


XS(XS_Object_GetType); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_GetType)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Object::GetType(THIS)");
	{
		Object *		THIS;
		int32		RETVAL;
		dXSTARG;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		RETVAL = THIS->GetType();
		XSprePUSH; PUSHu((UV)RETVAL);
	}
	XSRETURN(1);
}


XS(XS_Object_SetType); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_SetType)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: Object::SetType(THIS, type)");
	{
		Object *		THIS;
		int32		type = (int32)SvUV(ST(1));

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		THIS->SetType(type);
	}
	XSRETURN_EMPTY;
}


XS(XS_Object_GetIcon); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_GetIcon)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Object::GetIcon(THIS)");
	{
		Object *		THIS;
		int32		RETVAL;
		dXSTARG;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		RETVAL = THIS->GetIcon();
		XSprePUSH; PUSHu((UV)RETVAL);
	}
	XSRETURN(1);
}


XS(XS_Object_SetIcon); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_SetIcon)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: Object::SetIcon(THIS, icon)");
	{
		Object *		THIS;
		int32		icon = (int32)SvUV(ST(1));

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		THIS->SetIcon(icon);
	}
	XSRETURN_EMPTY;
}


XS(XS_Object_GetItemID); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_GetItemID)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Object::GetItemID(THIS)");
	{
		Object *		THIS;
		int32		RETVAL;
		dXSTARG;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		RETVAL = THIS->GetItemID();
		XSprePUSH; PUSHu((UV)RETVAL);
	}
	XSRETURN(1);
}


XS(XS_Object_SetItemID); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_SetItemID)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: Object::SetItemID(THIS, itemid)");
	{
		Object *		THIS;
		int32		itemid = (int32)SvUV(ST(1));

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		THIS->SetItemID(itemid);
	}
	XSRETURN_EMPTY;
}

XS(XS_Object_SetLocation); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_SetLocation)
{
	dXSARGS;
	if (items != 4)
		Perl_croak(aTHX_ "Usage: Object::SetLocation(THIS, x, y, z)");
	{
		Object *		THIS;
		float		x = (float)SvNV(ST(1));
		float		y = (float)SvNV(ST(2));
		float		z = (float)SvNV(ST(3));

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		THIS->SetLocation(x, y, z);
	}
	XSRETURN_EMPTY;
}

XS(XS_Object_SetX); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_SetX)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: Object::SetX(THIS, XPos)");
	{
		Object *		THIS;
		float		pos = (float)SvNV(ST(1));

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		THIS->SetX(pos);
	}
	XSRETURN_EMPTY;
}

XS(XS_Object_SetY); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_SetY)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: Object::SetY(THIS, YPos)");
	{
		Object *		THIS;
		float		pos = (float)SvNV(ST(1));

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		THIS->SetY(pos);
	}
	XSRETURN_EMPTY;
}

XS(XS_Object_SetZ); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_SetZ)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: Object::SetZ(THIS, ZPos)");
	{
		Object *		THIS;
		float		pos = (float)SvNV(ST(1));

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		THIS->SetZ(pos);
	}
	XSRETURN_EMPTY;
}

XS(XS_Object_SetHeading); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_SetHeading)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: Object::SetHeading(THIS, heading)");
	{
		Object *		THIS;
		float		heading = (float)SvNV(ST(1));

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		THIS->SetHeading(heading);
	}
	XSRETURN_EMPTY;
}

XS(XS_Object_SetModelName); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_SetModelName)
{
	dXSARGS;
	if (items < 1 || items > 2)
		Perl_croak(aTHX_ "Usage: Object::SetModelName(THIS, name)");
	{
		Object *		THIS;
		char *		name = NULL;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		if (items > 1)	{	name = (char *)SvPV_nolen(ST(1));	}

		THIS->SetModelName(name);
	}
	XSRETURN_EMPTY;
}
XS(XS_Object_GetModelName); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_GetModelName)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Object::GetModelName(THIS)");
	{
		Object *		THIS;
		Const_char *		RETVAL;
		dXSTARG;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		RETVAL = THIS->GetModelName();
		sv_setpv(TARG, RETVAL); XSprePUSH; PUSHTARG;
	}
	XSRETURN(1);
}

XS(XS_Object_Repop); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_Repop)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Object::Repop(THIS)");
	{
		Object *		THIS;
		Const_char *		RETVAL;
		dXSTARG;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");
		THIS->Repop();
	}
	XSRETURN_EMPTY;
}

XS(XS_Object_Depop); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_Depop)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Object::Depop(THIS)");
	{
		Object *		THIS;
		Const_char *		RETVAL;
		dXSTARG;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");
		THIS->Depop();
	}
	XSRETURN_EMPTY;
}


XS(XS_Object_GetEntityVariable); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_GetEntityVariable)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: Object::GetEntityVariable(THIS, id)");
	{
		Object *		THIS;
		int32		id = (int32)SvIV(ST(1));
		Const_char *		RETVAL;
		dXSTARG;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		RETVAL = THIS->GetEntityVariable(id);
		sv_setpv(TARG, RETVAL); XSprePUSH; PUSHTARG;
	}
	XSRETURN(1);
}

XS(XS_Object_EntityVariableExists); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_EntityVariableExists)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: Object::EntityVariableExists(THIS, id)");
	{
		Object *		THIS;
		int32		id = (int32)SvIV(ST(1));
		bool		RETVAL;

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		RETVAL = THIS->EntityVariableExists(id);
		ST(0) = boolSV(RETVAL);
		sv_2mortal(ST(0));
	}
	XSRETURN(1);
}

XS(XS_Object_SetEntityVariable); /* prototype to pass -Wmissing-prototypes */
XS(XS_Object_SetEntityVariable)
{
	dXSARGS;
	if (items != 3)
		Perl_croak(aTHX_ "Usage: Object::SetEntityVariable(THIS, id, var)");
	{
		Object *		THIS;
		int32		id = (int32)SvIV(ST(1));
		const char *	var = (const char *)SvPV_nolen(ST(2));

		if (sv_derived_from(ST(0), "Object")) {
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(Object *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type Object");
		if(THIS == NULL)
			Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

		THIS->SetEntityVariable(id, var);
	}
	XSRETURN_EMPTY;
}


#ifdef __cplusplus
extern "C"
#endif
XS(boot_Object); /* prototype to pass -Wmissing-prototypes */
XS(boot_Object)
{
	dXSARGS;
	char file[256];
	strncpy(file, __FILE__, 256);
	file[255] = 0;
	
	if(items != 1)
		fprintf(stderr, "boot_quest does not take any arguments.");
	char buf[128];

	//add the strcpy stuff to get rid of const warnings....



	XS_VERSION_BOOTCHECK ;
		newXSproto(strcpy(buf, "Depop"),XS_Object_Depop, file, "$");
		newXSproto(strcpy(buf, "Repop"),XS_Object_Repop, file, "$");
		newXSproto(strcpy(buf, "SetModelName"),XS_Object_SetModelName, file, "$$");
		newXSproto(strcpy(buf, "GetModelName"),XS_Object_GetModelName, file, "$");
		newXSproto(strcpy(buf, "GetX"),XS_Object_GetX, file, "$");
		newXSproto(strcpy(buf, "GetY"),XS_Object_GetY, file, "$");
		newXSproto(strcpy(buf, "GetZ"),XS_Object_GetZ, file, "$");
		newXSproto(strcpy(buf, "GetHeading"),XS_Object_GetHeading, file, "$");
		newXSproto(strcpy(buf, "SetX"),XS_Object_SetX, file, "$$");
		newXSproto(strcpy(buf, "SetY"),XS_Object_SetY, file, "$$");
		newXSproto(strcpy(buf, "SetZ"),XS_Object_SetZ, file, "$$");
		newXSproto(strcpy(buf, "SetHeading"),XS_Object_SetHeading, file, "$$");
		newXSproto(strcpy(buf, "SetLocation"),XS_Object_SetLocation, file, "$$$$");
		newXSproto(strcpy(buf, "SetItemID"),XS_Object_SetItemID, file, "$$");
		newXSproto(strcpy(buf, "GetItemID"),XS_Object_GetItemID, file, "$");
		newXSproto(strcpy(buf, "SetIcon"),XS_Object_SetIcon, file, "$$");
		newXSproto(strcpy(buf, "GetIcon"),XS_Object_GetIcon, file, "$");
		newXSproto(strcpy(buf, "SetType"),XS_Object_SetType, file, "$$");
		newXSproto(strcpy(buf, "GetType"),XS_Object_GetType, file, "$");
		newXSproto(strcpy(buf, "GetDBID"),XS_Object_GetDBID, file, "$");
		newXSproto(strcpy(buf, "ClearUser"),XS_Object_ClearUser, file, "$");
		newXSproto(strcpy(buf, "SetID"),XS_Object_SetID, file, "$$");
		newXSproto(strcpy(buf, "GetID"),XS_Object_GetID, file, "$");
		newXSproto(strcpy(buf, "Save"),XS_Object_Save, file, "$");
		newXSproto(strcpy(buf, "VarSave"),XS_Object_VarSave, file, "$");
		newXSproto(strcpy(buf, "DeleteItem"),XS_Object_DeleteItem, file, "$$");
		newXSproto(strcpy(buf, "StartDecay"),XS_Object_StartDecay, file, "$$");
		newXSproto(strcpy(buf, "Delete"),XS_Object_Delete, file, "$$");
		newXSproto(strcpy(buf, "IsGroundSpawn"),XS_Object_IsGroundSpawn, file, "$");
		newXSproto(strcpy(buf, "Close"),XS_Object_Close, file, "$");
		newXSproto(strcpy(buf, "GetEntityVariable"), XS_Object_GetEntityVariable, file, "$$");
		newXSproto(strcpy(buf, "SetEntityVariable"), XS_Object_SetEntityVariable, file, "$$$");
		newXSproto(strcpy(buf, "EntityVariableExists"), XS_Object_EntityVariableExists, file, "$$");
	XSRETURN_YES;
}
#endif //EMBPERL_XS_CLASSES
