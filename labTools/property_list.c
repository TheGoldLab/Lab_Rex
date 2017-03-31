/*
** PROPERTY_LIST.C
**
**	Routines for creating/using/freeing
**		Properties	(name, value, etc)
**		Lists			(arrays of properties)
**		Groups 		(arrays of lists)
*/

#include <stdio.h>
#include "property_list.h"
#include "toys.h"

/* PUBLIC ROUTINES */

/* PUBLIC ROUTINE: pl_group_init
**
*/
_PLgroup pl_group_init(char *name, int id)
{
	_PLgroup group 		= SAFE_STALLOC(_PLgroup_struct);

	if(name)
		group->name 		= TOY_CARBONCOPY(name);
	else
		group->name			= TOY_CARBONCOPY("group");

	group->id				= id;
	group->lists			= NULL;
	group->lists_length 	= 0;

	return(group);
}

/* PUBLIC ROUTINE: pl_group_add_list
**
*/
void pl_group_add_list(_PLgroup group, _PLlist list)
{
	if(!group || !list)
		return;

	/* allocate a new pointer in the array */
	if(!group->lists)

		/* start the array */
		group->lists = (_PLlist *) 
			SAFE_MALLOC(sizeof(_PLlist));
	
	else

		/* realloc (append) the array */
		group->lists = (_PLlist *) SAFE_REALLOC(
			group->lists, ((group->lists_length + 1) *
			sizeof(_PLlist)));

	/* add the list */
	group->lists[group->lists_length++] = list;	
}

/* PUBLIC ROUTINE: pl_group_get_list
**
**	Returns named list from the given group.
*/
_PLlist pl_group_get_list(_PLgroup group, char *name)
{
	register int i;

	if(!group || !name)
		return(NULL);

	for(i=0;i<group->lists_length;i++)
		if(!strcmp(group->lists[i]->name, name))
			return(group->lists[i]);

	return(NULL);
}

/* PUBLIC ROUTINE: pl_group_get_listi
**
**	Returns index of named list from the given group.
*/
int pl_group_get_listi(_PLgroup group, char *name)
{
	register int i;

	if(!group || !name)
		return(NULL);

	for(i=0;i<group->lists_length;i++)
		if(!strcmp(group->lists[i]->name, name))
			return(i);

	return(-1);
}

/* PUBLIC ROUTINE: pl_group_get_prop
**
*/
_PLproperty pl_group_get_prop(_PLgroup group, char *lname, char *pname)
{
	register int i;
	_PLlist *lists;

	if(!group)
		return(NULL);

	/* find the list */
	lists = group->lists;
	for(i=group->lists_length;i>0;i--,lists++)
		if(!strcmp((*lists)->name, lname))
			return(pl_list_get_prop(*lists, pname));

	return(NULL);
}

/* PUBLIC ROUTINE: pl_group_get_v
**
**	Gets the value (index 0) of a named property from
**		a named list in the given group
*/
valtype pl_group_get_v(_PLgroup group, char *lname, char *pname)
{
	register int i;
	_PLlist *lists;

	if(!group)
		return(PL_NOVAL);

	/* find the list */
	lists = group->lists;
	for(i=group->lists_length;i>0;i--,lists++)
		if(!strcmp((*lists)->name, lname))
			return(pl_list_get_v(*lists, pname));

	return(PL_NOVAL);
}

/* PUBLIC ROUTINE: pl_group_get_w
**
**	Gets the working copy of the value 
**		(index 0) of a named property from
**		a named list in the given group
*/
valtype pl_group_get_w(_PLgroup group, char *lname, char *pname)
{
	register int i;
	_PLlist *lists;

	if(!group)
		return(PL_NOVAL);

	/* find the list */
	lists = group->lists;
	for(i=group->lists_length;i>0;i--,lists++)
		if(!strcmp((*lists)->name, lname))
			return(pl_list_get_w(*lists, pname));

	return(PL_NOVAL);
}

/* PUBLIC ROUTINE: pl_group_get_vi
**
**	Gets the value with the given index of a named property from
**		a named list in the given group
*/
valtype pl_group_get_vi(_PLgroup group, char *lname, char *pname, int index)
{
	register int i;
	_PLlist *lists;

	if(!group)
		return(PL_NOVAL);

	/* find the list */
	lists = group->lists;
	for(i=group->lists_length;i>0;i--,lists++)
		if(!strcmp((*lists)->name, lname))
			return(pl_list_get_vi(*lists, pname, index));

	return(PL_NOVAL);
}

/* PUBLIC ROUTINE: pl_group_get_wi
**
**	Gets the working copy of the value 
**		of a named property from
**		a named list in the given group
*/
valtype pl_group_get_wi(_PLgroup group, char *lname, char *pname, int index)
{
	register int i;
	_PLlist *lists;

	if(!group)
		return(PL_NOVAL);

	/* find the list */
	lists = group->lists;
	for(i=group->lists_length;i>0;i--,lists++)
		if(!strcmp((*lists)->name, lname))
			return(pl_list_get_wi(*lists, pname, index));

	return(PL_NOVAL);
}

/* PUBLIC ROUTINE: pl_group_set_v2w
**
*/
void pl_group_set_v2w(_PLgroup group)
{
	register int i;

	if(!group)
		return;

	for(i=0;i<group->lists_length;i++)
		pl_list_set_v2w(group->lists[i]);
}

/* PUBLIC ROUTINE: pl_group_set_w2v
**
*/
void pl_group_set_w2v(_PLgroup group)
{
	register int i;

	if(!group)
		return;

	for(i=0;i<group->lists_length;i++)
		pl_list_set_w2v(group->lists[i]);
}

/* PUBLIC ROUTINE: pl_group_clear_w
**
*/
void pl_group_clear_w(_PLgroup group)
{
	register int i;

	if(!group)
		return;

	for(i=0;i<group->lists_length;i++)
		pl_list_clear_w(group->lists[i]);
}

/* PUBLIC ROUTINE: pl_group_print
**
*/
void pl_group_print(_PLgroup group)
{
	register int i;

	printf("---- Group <%s>, %d lists ----\n", 
		group->name ? group->name : "group", group->lists_length);

	for(i=0;i<group->lists_length;i++)
		pl_list_print(group->lists[i]);

	printf("------------------------------\n");
}

/* PUBLIC ROUTINE: pl_group_free
**
*/
void pl_group_free(_PLgroup group)
{
	register int i;

	if(!group)
		return;

	/* free each list */
	for(i=0;i<group->lists_length;i++)
		pl_list_free(group->lists[i]);

	/* free the rest */
	SAFE_FREE(group->lists);
	SAFE_FREE(group->name);
}

/* PUBLIC ROUTINE: pl_list_init
**
*/
_PLlist pl_list_init(char *name, int id, int values_length)
{
	_PLlist list = SAFE_STALLOC(_PLlist_struct);

	list->name 					= TOY_CARBONCOPY(name);
	list->id	  					= id;

	list->properties 			= NULL;
	list->properties_length = 0;
	list->values_length		= values_length;

	return(list);
}

/* PUBLIC ROUTINE: pl_list_initV
**
** va_arg is NULL-terminated list of triples:
**		char 		*name
**		valtype 	value
**		double	 	multiplier
*/
_PLlist pl_list_initV(char *name, int id, int values_length, ...)
{
	_PLlist list = pl_list_init(name, id, values_length);
	register int i;
	va_list ap;
	char *buf;
	valtype value;
	double multiplier;

	va_start(ap, values_length);
	while(buf = va_arg(ap, char *)) {

		/* allocate a new pointer in the array */
		if(!list->properties)

			/* start the array */
			list->properties = (_PLproperty *) 
				SAFE_MALLOC(sizeof(_PLproperty));
	
		else

			/* realloc (append) the array */
			list->properties = (_PLproperty *) SAFE_REALLOC(
				list->properties, ((list->properties_length + 1) *
				sizeof(_PLproperty)));

		/* add the property & increment the count */
		value 		= va_arg(ap, valtype);
		multiplier 	= va_arg(ap, double);
		list->properties[list->properties_length++] = 
			pl_prop_init(buf, value, values_length, multiplier);
	}

	/* clean up */
	va_end(ap);
	return(list);
}	

/* PUBLIC ROUTINE: pl_list_copy
**
**	Arguments:
**		in	 			... the list to copy
**		num_values 	... the number of values to create in the new copy
**								(see pl_prop_copy for details) 
*/
_PLlist pl_list_copy(_PLlist in, int num_values)
{
	_PLlist out;
	register int i;

	if(!in)
		return(NULL);

	/* init the new list */
	out = pl_list_init(in->name, in->id, 
		num_values > 0 ? num_values : in->values_length);

	/* add the properties */
	out->properties_length = in->properties_length;	

	out->properties = SAFE_ZALLOC(_PLproperty, 
		out->properties_length);

	for(i=0;i<out->properties_length;i++)
		out->properties[i] = pl_prop_copy(in->properties[i],
														out->values_length);

	return(out);
}

/* PUBLIC ROUTINE: pl_list_append
**
**	Append a property to a list.
**
**	Arguments:
**		char 		*name
**		valtype	value
**		double 	multiplier
*/
void pl_list_append(_PLlist list, char *name, valtype value, 
			double multiplier)
{
	register int index;
	
	/* allocate a new pointer in the array */
	if(!list->properties)

		/* start the array */
		list->properties = (_PLproperty *) 
			SAFE_MALLOC(sizeof(_PLproperty));
	
	else

		/* realloc (append) the array */
		list->properties = (_PLproperty *) SAFE_REALLOC(
			list->properties, ((list->properties_length + 1) *
			sizeof(_PLproperty)));

	/* add the property & increment the count */
	list->properties[list->properties_length++] = 
		pl_prop_init(name, value, list->values_length, multiplier);
}	

/* PUBLIC ROUTINE: pl_list_appendV
**
** va_arg is NULL-terminated list of triples:
**		char 		*name
**		valtype	value
**		double 	multiplier
*/
void pl_list_appendV(_PLlist list, ...)
{
	register int i;
	va_list ap;
	char *name;
	valtype value;
	double multiplier;
	
	va_start(ap, list);
	while(name = va_arg(ap, char *)) {

		/* allocate a new pointer in the array */
		if(!list->properties)

			/* start the array */
			list->properties = (_PLproperty *) 
				SAFE_MALLOC(sizeof(_PLproperty));
	
		else

			/* realloc (append) the array */
			list->properties = (_PLproperty *) SAFE_REALLOC(
				list->properties, ((list->properties_length + 1) *
				sizeof(_PLproperty)));

		/* add the property & increment the count */
		value 		= va_arg(ap, valtype);
		multiplier 	= va_arg(ap, double);
		list->properties[list->properties_length++] = 
			pl_prop_init(name, value, 1, multiplier);
	}

	/* clean up */
	va_end(ap);
}	

/* PUBLIC ROUTINE: pl_list_cat
**
**	Careful -- this assumes both lists have the
**		save "values_length"
*/
void pl_list_cat(_PLlist list1, _PLlist list2)
{
	register int i,j;

	/* if no properties in list 2, do nothing * /
	if(!list2 || !list2->properties)
		return;

	/* if list1 is empty, set to list 2 */
	if(!list1) {
		list1 = list2;
		return;
	}

	/* if list1 properties are empty, set to properties
	**		of list 2
	*/
	if(!list1->properties) {
		list1->properties 			= list2->properties;
		list1->properties_length 	= list2->properties_length;
		list1->values_length 		= list2->values_length;

		/* free the remainder of the list 2 struct */
		SAFE_FREE(list2->name);
		SAFE_FREE(list2);
	
		return;
	}	

	/* REALLOC the pointer array in list 1 and copy the
	**		pointers from list 2
	*/
	list1->properties = (_PLproperty *) SAFE_REALLOC(
				list1->properties, (list1->properties_length +
				list2->properties_length) * sizeof(_PLproperty));

	for(i=list1->properties_length,j=0;j<list2->properties_length;i++,j++)
		list1->properties[i] = list2->properties[j];

	list1->properties_length = list1->properties_length +
				list2->properties_length;

	/* free the remainder of the list 2 struct */
	SAFE_FREE(list2->properties);
	SAFE_FREE(list2->name);
	SAFE_FREE(list2);
}

/* PUBLIC ROUTINE: pl_list_get_prop
**
** Returns named property from given list.
*/
_PLproperty pl_list_get_prop(_PLlist list, char *name)
{
	register int i;
	_PLproperty *props;

	if(!list || !name)
		return(NULL);

	props = list->properties;
	for(i=list->properties_length;i>0;i--,props++)
		if(!strcmp((*props)->name, name))
			return(*props);

	return(NULL);
}

/* PUBLIC ROUTINE: pl_list_get_propi
**
** Returns index of named property from given list.
**
*/
int pl_list_get_propi(_PLlist list, char *name)
{
	register int i;

	if(!list || !name)
		return(NULL);

	for(i=0;i<list->properties_length;i++)
		if(!strcmp(list->properties[i]->name, name))
			return(i);

	return(-1);
}

/* PUBLIC ROUTINE: pl_list_get_v
**
*/
valtype pl_list_get_v(_PLlist list, char *name)
{
	_PLproperty prop;

	if(prop = pl_list_get_prop(list, name))
		return(prop->values[0]);

	return(PL_NOVAL);
}

/* PUBLIC ROUTINE: pl_list_get_w
**
*/
valtype pl_list_get_w(_PLlist list, char *name)
{
	_PLproperty prop;

	if(prop = pl_list_get_prop(list, name))
		return(prop->values_wc[0]);

	return(PL_NOVAL);
}

/* PUBLIC ROUTINE: pl_list_get_vi
**
*/
valtype pl_list_get_vi(_PLlist list, char *name, int value_index)
{
	_PLproperty prop;

	if(prop = pl_list_get_prop(list, name))
		return(prop->values[value_index]);

	return(PL_NOVAL);
}

/* PUBLIC ROUTINE: pl_list_get_wi
**
*/
valtype pl_list_get_wi(_PLlist list, char *name, int value_index)
{
	_PLproperty prop;

	if(prop = pl_list_get_prop(list, name))
		return(prop->values_wc[value_index]);

	return(PL_NOVAL);
}

/* PUBLIC ROUTINE: pl_list_set_v
**
** Sets value (index 0) of the named property in the list.
*/
void pl_list_set_v(_PLlist list, char *name, valtype val)
{
	_PLproperty prop;

	if(!(prop = pl_list_get_prop(list, name)))
		return;

	prop->values[0] = val;
}

/* PUBLIC ROUTINE: pl_list_set_w
**
** Sets working copy (index 0) of the named property in the list.
*/
void pl_list_set_w(_PLlist list, char *name, valtype val)
{
	_PLproperty prop;

	if(!(prop = pl_list_get_prop(list, name)))
		return;

	prop->values_wc[0] = val;
}

/* PUBLIC ROUTINE: pl_list_set_vi
**
** Sets value of given index of the 
**		named property in the list.
*/
void pl_list_set_vi(_PLlist list, char *name, int index, valtype val)
{
	_PLproperty prop;

	if(!(prop = pl_list_get_prop(list, name)))
		return;

	if(index < 0 || index >= prop->values_length)
		index = 0;

	prop->values[index] = val;
}
/* PUBLIC ROUTINE: pl_list_set_wi
**
** Sets working copy of given index of the 
**		named property in the list.
*/
void pl_list_set_wi(_PLlist list, char *name, int index, valtype val)
{
	_PLproperty prop;

	if(!(prop = pl_list_get_prop(list, name)))
		return;

	if(index < 0 || index >= prop->values_length)
		index = 0;

	prop->values_wc[index] = val;
}

/* PUBLIC ROUTINE: pl_list_set_v2w
**
*/
void pl_list_set_v2w(_PLlist list)
{
	register int i;

	if(!list)
		return;

	for(i=0;i<list->properties_length;i++)
		pl_prop_set_v2w(list->properties[i]);
}

/* PUBLIC ROUTINE: pl_list_set_w2v
**
*/
void pl_list_set_w2v(_PLlist list)
{
	register int i;

	if(!list)
		return;

	for(i=0;i<list->properties_length;i++)
		pl_prop_set_w2v(list->properties[i]);
}

/* PUBLIC ROUTINE: pl_list_clear_w
**
*/
void pl_list_clear_w(_PLlist list)
{
	register int i;

	if(!list)
		return;

	for(i=0;i<list->properties_length;i++)
		pl_prop_clear_w(list->properties[i]);
}

/* PUBLIC ROUTINE: pl_list_print
**
*/
void pl_list_print(_PLlist list)
{
	register int i;

	printf(" -- LIST %s, %d items, %d values:\n", 
		list->name, list->properties_length, list->values_length);

	for(i=0;i<list->properties_length;i++)
		pl_prop_print(list->properties[i]);
}

/* PUBLIC ROUTINE: pl_list_free
**
*/
void pl_list_free(_PLlist list)
{
	register int i;

	if(!list)
		return;

	/* free each property */
	for(i=0;i<list->properties_length;i++)
		pl_prop_free(list->properties[i]);

	/* free the rest */
	SAFE_FREE(list->properties);
	SAFE_FREE(list->name);
	SAFE_FREE(list);
}

/* PUBLIC ROUTINE: pl_lists_copyIV
**
**	Copies property values from one list
**		to another. 
**
**		I for "Indexed" (values/objects)
**		V for "va_list" (property names)
**
** "Indices" are *_n, *_is variable, as follows:
**		-1, NULL	... use all values
**		i,  NULL	... use value i
**		n,  {i1, i2, ...} ... use indices
*/
void pl_lists_copyIV(
		_PLlist from, 	char from_type, int from_n, int *from_is, 
		_PLlist to, 	char to_type,   int to_n, 	 int *to_is, ...)
{
	register int i;
	_PLproperty prop;
	va_list ap;
	char *arg;

	/* start valist */
	va_start(ap, to_is);
	arg = va_arg(ap, char *);

	/* Check first va_arg ... special case of just NULL
	**	means to use all properties that exist in both
	*/
	if(!arg) {

		for(i=0;i<from->properties_length;i++) {

			if(prop = pl_list_get_prop(to, from->properties[i]->name))	
				pl_props_copyI(
					from->properties[i], from_type, from_n, from_is,
					prop, 					to_type,   to_n,   to_is);
		}

	} else {

		while(arg) {	

			pl_props_copyI(
				pl_list_get_prop(from, arg), from_type, from_n, from_is,
				pl_list_get_prop(to, arg),   to_type,   to_n,   to_is);

			/* get next arg */
			arg = va_arg(ap, char *);
		}
	}

	/* cleanup va_list */
	va_end(ap);
}

/* PUBLIC ROUTINE: pl_ecodes_by_nameV
**
**	Sends a property value as a "tagged" ecode ... 
**	that is, a pair of codes, where the first is 
**	the "tag", the second is the value
**
** Arguments:
**		list	... the list (e.g., a dXobject)
**		base  ... value added to the value
**		va_args ... triples of
**						(string) name
**						(int) 	value index
**						(int)		tag
*/
void pl_ecodes_by_nameV(_PLlist list, int base, ...)
{
	va_list ap;
	char *property_name;
	_PLproperty prop;
	int index, tag;

	if(!list)
		return;

	/* start the va_list */
	va_start(ap, base);

	/* loop through the property names */
	while((property_name = va_arg(ap, char *)) != NULL) {

		/* get the property */
		prop = pl_list_get_prop(list, property_name);

		/* get the index */
		index = va_arg(ap, int);

		/* get the "tag" code */
		tag = va_arg(ap, int);

		/* send the tagged value, if found */
		if(prop) {
			ec_send_code_tagged(tag, base + prop->values[index]);
/* printf("pl_ecodes_by_name: tag: %d, value: %d\n",tag, base + prop->values[index]);		 */
		}	else {
			printf("pl_ecodes_by_name: bad property <%s>\n", property_name);
		}
	}

	/* end the va_list */
	va_end(ap);
}

/* PUBLIC ROUTINE: pl_ecodes_by_indexV
**
*/
void pl_ecodes_by_indexV(_PLlist list, int base, ...)
{
	va_list ap;
	_PLproperty prop;
	int property_index, value_index, tag;

	if(!list)
		return;

	/* start the va_list */
	va_start(ap, base);

	/* loop through the property names */
	while((property_index = va_arg(ap, int)) != ENDI) {

		/* get the property */
		prop = list->properties[property_index];

		/* get the index */
		value_index = va_arg(ap, int);

		/* get the "tag" code */
		tag = va_arg(ap, int);

		/* send the tag, value LOW priority */
		ec_send_code_lo(tag);
		ec_send_code_lo(base + prop->values[value_index]);
	}

	/* end the va_list */
	va_end(ap);
}

	/* _PLproperty */

/* PUBLIC ROUTINE: pl_prop_init
**
*/
_PLproperty pl_prop_init (char *name, valtype value, 
			int length, double multiplier)
{
	_PLproperty prop = SAFE_STALLOC(_PLproperty_struct);
	register int i;

	/* name */
	if(name)
		prop->name 			= TOY_CARBONCOPY(name);
	else
		prop->name 			= NULL;

	/* values (and working copies) */
	prop->values_length 	= length;

	if(length > 0) {
	
		prop->values 		= SAFE_ZALLOC(valtype, length);
		prop->values_wc 	= SAFE_ZALLOC(valtype, length);

		for(i=0;i<length;i++) {
			prop->values[i] 	 = value;
			prop->values_wc[i] = value;
		}

	} else {
		
		prop->values		= NULL;
		prop->values_wc	= NULL;
	}

	prop->multiplier 		= multiplier;

	return(prop);
}

/* PUBLIC ROUTINE: pl_prop_copy
**
** Makes an exact copy, with both 
**		values and working copies...
**	Note that the only strangeness is with the
**		"num_values" input argument: if negative, it's
**		just a flag indicating that ALL values/wcs should
**		be copied; otherwise it says how many copies of the
**		values/wcs should be made.
*/
_PLproperty pl_prop_copy(_PLproperty in, int num_values)
{
	_PLproperty out;
	register int i;

	/* check given _PLproperty */
	if(!in)	
		return(NULL);

	/* make the new struct */
 	out = SAFE_STALLOC(_PLproperty_struct);

	/* copy name */
	if(in->name)
		out->name = TOY_CARBONCOPY(in->name);
	else
		out->name = NULL;

	/* copy values */
	if(num_values < 0)
		out->values_length = in->values_length;
	else
		out->values_length = num_values;

	if(!out->values_length) {

		out->values 	= NULL;
		out->values_wc = NULL;

	} else {

		out->values 	= SAFE_ZALLOC(valtype, out->values_length);
		out->values_wc = SAFE_ZALLOC(valtype, out->values_length);

		/* copy the array */
		if(out->values_length == in->values_length) {
			for(i=0;i<out->values_length;i++) {
				out->values[i] 	= in->values[i];
				out->values_wc[i] = in->values_wc[i];
			}

		/* Otherwise make copies of the first value/wc into the
		**		new array
		*/
		} else {
			for(i=0;i<out->values_length;i++) {
				out->values[i] 	= in->values[0];
				out->values_wc[i] = in->values_wc[0];
			}
		}
	}

	/* multiplier */
	out->multiplier = in->multiplier;	

	return(out);
}

/* PUBLIC ROUTINE: pl_prop_set_v2w
**
*/
void pl_prop_set_v2w(_PLproperty prop)
{
	register int i;

	for(i=0;i<prop->values_length;i++)
		prop->values_wc[i] = prop->values[i];
}

/* PUBLIC ROUTINE: pl_prop_set_w2v
**
*/
void pl_prop_set_w2v(_PLproperty prop)
{
	register int i;

	for(i=0;i<prop->values_length;i++)
		prop->values[i] = prop->values_wc[i];
}

/* PUBLIC ROUTINE: pl_prop_clear_w
**
*/
void pl_prop_clear_w(_PLproperty prop)
{
	register int i;

	for(i=0;i<prop->values_length;i++)
		prop->values_wc[i] = 0;
}

/* PUBLIC ROUTINE: pl_prop_print
**
*/
void pl_prop_print(_PLproperty prop)
{	
	register int i;

	printf("%11.10s : ", prop->name);

	for(i=0;i<prop->values_length;i++) {
		printf("%5d", prop->values[i]);
		if(i<prop->values_length-1)
			printf(", ");
	}

	printf("\n");
}

/* PUBLIC ROUTINE: pl_prop_free
**
*/
void pl_prop_free(_PLproperty prop)
{
	if(!prop)
		return;

	SAFE_FREE(prop->values);
	SAFE_FREE(prop->values_wc);

	SAFE_FREE(prop);
}

/* PUBLIC ROUTINE: pl_props_copyI
**
*/
void pl_props_copyI(
	_PLproperty from, char from_type, int from_n, int *from_is,
	_PLproperty to,   char to_type,   int to_n, int *to_is)
{
	register int i;
	valtype *fromPtr, *toPtr;

	fromPtr = from_type == 'v' ? from->values : from->values_wc;
	toPtr   = to_type   == 'v' ? to->values 	: to->values_wc;

	if(!from_is) {

		if(from_n < 0) {

			/* use all values */
			for(i=from->values_length;i>0;i--)
				*(toPtr++) = *(fromPtr++);

		} else {

			/* use given values (*_n = indices) */
			*(toPtr + to_n) = *(fromPtr + from_n);
		}

	} else {

		for(i=0;i<from_n;i++)
			*(toPtr + to_is[i]) = *(fromPtr + from_is[i]);
	}	
}

/* PUBLIC ROUTINE: pl_init_to_group
**
*/
_PLgroup pl_init_to_group(_PLinit def)
{
	_PLgroup group = pl_group_init(NULL, 0);
	_PLlist  list;

	while(def->name) {

		/* make the list */
		list = pl_init_to_list(def);

		/* add to group */
		pl_group_add_list(group, list);

		/* increment index. yuk. */
		def += (list->properties_length + 2);
	}

	/* return the group */
	return(group);
}

/* PUBLIC ROUTINE: pl_init_to_list
**
*/
_PLlist pl_init_to_list(_PLinit init)
{
	_PLlist list;
	int i = 0;

	if(!init->name)
		return(NULL);

	/* The "value" given with the list name is actually
	**		the number of values per property
	*/
	list = pl_list_init(init->name, 0, 
			init->value > 0 ? init->value : 1);

	/* loop through the properties */
	while((++init)->name)
		pl_list_append(list, init->name,
				init->value, init->multiplier);

	/* return the list (pointer) */
	return(list);
}
