/*
** PROPERTY_LIST.H
**
** Header file for property_list.c.
**
** Data structures and routine prototypes for
**	 creating/using/freeing:
**    Properties  (name, value, etc)
**    Lists       (arrays of properties)
**    Groups      (arrays of lists)
*/

#ifndef PROPERTY_LIST_H_
#define PROPERTY_LIST_H_

/* INCLUDES */
#include <stdarg.h>
#include <sys/types.h>
#include "../hdr/rexHdr.h"

/* DATA STRUCTURES */

/* these are the data structures defined in this file
**
**	_PLinit 		-- convenience struct for initializing lists & groups
**	_PLproperty -- arbitarary property; name + value
**	_PLlist 		-- list (array) of properties
**	_PLgroup 	-- array of _PLlists
**
*/
typedef struct _PLinit_struct 		*_PLinit;
typedef struct _PLproperty_struct 	*_PLproperty;
typedef struct _PLlist_struct 		*_PLlist;
typedef struct _PLgroup_struct 		*_PLgroup;

/* And finally the data type
*/
typedef int		valtype;

/* _PLinit_struct
**
**	For defining a _PLinit_struct
*/
struct _PLinit_struct {
	char 			*name;
	valtype		value;
	double		multiplier;
};

/* _PLproperty_struct
**
*/
struct _PLproperty_struct {

	char		   *name;

	/* value + working copy
	**	This bit of tomfoolery is because
	**		these are usually used in Rex Menus,
	**		and we might want to update/recompute
	**		temporarily without affecting the menu.
	*/
	valtype	   *values;
	valtype		*values_wc;			/* "working copy" */
	valtype		 values_length;

	/* Property values are long ints,
	** 	consistent with Rex functionality;
	**		"multiplier" is needed to convert
	**		to double, typically used for sending
	** 	true values to Matlab
	*/
	double		 multiplier;
};

/* _PLlist_struct
**
**	List of properties.
**	Note that each property can
**	have an array of values; the
**	size of that array can be
**	the same for all properties
**	in a list; thus we store that
**	size here, for convenience.
*/
struct _PLlist_struct {
	char			*name;
	int			 id;

	_PLproperty *properties;
	int			 properties_length;
	int			 values_length; 
};

/* _PLgroup_struct
**
** Array of lists.
*/
struct _PLgroup_struct {
	char		  *name;
	int			id;

	_PLlist	  *lists;
	int		 	lists_length;
};

/* PUBLIC MACROS  */
	/* get property value/wc */
#define PL_PV(p,vi) 	((double)((p)->values[(vi)])*(p)->multiplier)
#define PL_PVR(p,vi) ((valtype)((p)->values[(vi)]))
#define PL_PW(p,vi) 	((double)((p)->values_wc[(vi)])*(p)->multiplier)
#define PL_PWR(p,vi) ((valtype)((p)->values_wc[(vi)]))

	/* property value from a list, by index */
#define PL_L2PV(l,pi)			(l)->properties[(pi)]->values[0]
#define PL_L2PW(l,pi)			(l)->properties[(pi)]->values_wc[0]
#define PL_L2PVS(l,pi,vi)		(l)->properties[(pi)]->values[(vi)]
#define PL_L2PWS(l,pi,wi)		(l)->properties[(pi)]->values_wc[(wi)]

	/* property value from a group, by indices */
#define PL_G2PV(g,li,pi)		PL_L2PV((g)->lists[(li)],(pi))
#define PL_G2PW(g,li,pi)		PL_L2PW((g)->lists[(li)],(pi))
#define PL_G2PVS(g,li,pi,vi)	PL_L2PVS((g)->lists[(li)],(pi),(vi))
#define PL_G2PWS(g,li,pi,vi)	PL_L2PWS((g)->lists[(li)],(pi),(vi))

	/* conditionally set property (if current value == NULLI)  */
#define PL_NOVAL NULLI
#define PL_CSET(v1, v2) if((v1) == NULLI) (v1) = (v2)
//#define PL_CSET(v1, v2)  (v1) = (v2)

/* PUBLIC ROUTINE PROTOTYPES */

	/* _PLgroup */
_PLgroup		pl_group_init		(char *, int);
void			pl_group_add_list	(_PLgroup, _PLlist);
_PLlist		pl_group_get_list	(_PLgroup, char *);
int			pl_group_get_listi(_PLgroup, char *);
_PLproperty	pl_group_get_prop	(_PLgroup, char *, char *);
valtype		pl_group_get_v		(_PLgroup, char *, char *);
valtype		pl_group_get_w		(_PLgroup, char *, char *);
valtype		pl_group_get_vi	(_PLgroup, char *, char *, int);
valtype		pl_group_get_wi	(_PLgroup, char *, char *, int);
void			pl_group_set_v2w	(_PLgroup);
void			pl_group_set_w2v	(_PLgroup);
void			pl_group_clear_w	(_PLgroup);
void			pl_group_print		(_PLgroup);
void			pl_group_free		(_PLgroup);

	/* _PLlist */
_PLlist		pl_list_init		(char *, int, int);
_PLlist		pl_list_initV		(char *, int, int, ...);
_PLlist		pl_list_copy		(_PLlist, int); 
void			pl_list_append		(_PLlist, char *, valtype, double);
void			pl_list_appendV	(_PLlist, ...);
void			pl_list_cat			(_PLlist, _PLlist);
_PLproperty	pl_list_get_prop	(_PLlist, char *);
int			pl_list_get_propi	(_PLlist, char *);
valtype		pl_list_get_v		(_PLlist, char *);
valtype		pl_list_get_w		(_PLlist, char *);
valtype		pl_list_get_vi		(_PLlist, char *, int);
valtype		pl_list_get_wi		(_PLlist, char *, int);
void			pl_list_set_v		(_PLlist, char *, valtype);
void			pl_list_set_w		(_PLlist, char *, valtype);
void			pl_list_set_vi		(_PLlist, char *, int, valtype);
void			pl_list_set_wi		(_PLlist, char *, int, valtype);
void			pl_list_set_v2w	(_PLlist);
void			pl_list_set_w2v	(_PLlist);
void			pl_list_clear_w	(_PLlist);
void			pl_list_print		(_PLlist);
void			pl_list_free		(_PLlist);
void			pl_lists_copyIV	(_PLlist, char, int, int *,
										 _PLlist, char, int, int *, ...);

void			pl_ecodes_by_nameV (_PLlist, int, ...);
void			pl_ecodes_by_indexV(_PLlist, int, ...);

	/* _PLproperty */
_PLproperty	pl_prop_init		(char *, int, int, double);
_PLproperty	pl_prop_copy		(_PLproperty, int);
void			pl_prop_set_v2w	(_PLproperty);
void			pl_prop_set_w2v	(_PLproperty);
void			pl_prop_clear_w	(_PLproperty);
void			pl_prop_print		(_PLproperty);
void			pl_prop_free		(_PLproperty);
void			pl_props_copyI		(_PLproperty, char, int, int *,
										 _PLproperty, char, int, int *);

	/* _PLinit */
_PLgroup		pl_init_to_group	(_PLinit);
_PLlist		pl_init_to_list	(_PLinit);

#endif /* PROPERTY_LIST_H_ */
