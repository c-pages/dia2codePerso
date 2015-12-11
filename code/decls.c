#include "decls.h"

declaration *decls = NULL;
static namelist tmp_classes = NULL;

static char *sscanfmt()
{
    static char buf[16];
    sprintf (buf, "#%%%d[^#]#", SMALL_BUFFER - 1);
    return buf;
}


int use_corba = 0;

module *
create_nested_modules_from_pkglist (umlpackagelist pkglist, module *m)
{
    /* Expects pkglist and m to be non-NULL and m->contents to be NULL.
       Returns a reference to the innermost module created.  */
    while (pkglist->next != NULL) {
        declaration *d = NEW (declaration);
        d->decl_kind = dk_module;
        d->prev = d->next = NULL;
        d->u.this_module = NEW (module);
        m->contents = d;
        pkglist = pkglist->next;
        m = d->u.this_module;
        m->pkg = pkglist->key;
        m->contents = NULL;
    }
    return m;
}

module *
find_or_add_module (declaration **dptr, umlpackagelist pkglist)
{
    declaration *d = *dptr;
    module *m;

    if (pkglist == NULL)
        return NULL;
    if (d == NULL) {
        *dptr = NEW (declaration);
        d = *dptr;
    } else {
        declaration *dprev = NULL;
        while (d != NULL) {
            if (d->decl_kind == dk_module &&
                !strcmp (d->u.this_module->pkg->name, pkglist->key->name)) {
                m = d->u.this_module;
                if (pkglist->next == NULL)
                    return m;
                if (m->contents == NULL) {
                    return create_nested_modules_from_pkglist (pkglist, m);
                }
                return find_or_add_module (&m->contents, pkglist->next);
            }
            dprev = d;
            d = d->next;
        }
        if (dprev != NULL) {
            dprev->next = NEW (declaration);
            d = dprev->next;
        }
    }
    d->decl_kind = dk_module;
    d->next = NULL;
    d->u.this_module = NEW (module);
    m = d->u.this_module;
    m->pkg = pkglist->key;
    m->contents = NULL;
    return create_nested_modules_from_pkglist (pkglist, m);
}

module *
find_module (declaration *d, umlpackagelist pkglist)
{
    while (d != NULL) {
        if (d->decl_kind == dk_module) {
            module *m = d->u.this_module;
            if (!strcmp (m->pkg->name, pkglist->key->name)) {
                if (pkglist->next != NULL)
                    return find_module (m->contents, pkglist->next);
                else
                    return m;
            }
        }
        d = d->next;
    }
    return NULL;
}

declaration *
find_class (umlclassnode *node)
{
    declaration *d;

    if (node->key->package != NULL) {
        umlpackagelist pkglist = make_package_list (node->key->package);
        module *m = find_module (decls, pkglist);
        if (m == NULL || m->contents == NULL)
            return 0;
        d = m->contents;
    } else {
        d = decls;
    }

    while (d != NULL) {
        if (d->decl_kind == dk_class) {
            umlclassnode *cl = d->u.this_class;
            if (!strcmp (cl->key->name, node->key->name))
                return d;
        }
        d = d->next;
    }
    return NULL;
}

declaration *
append_decl (declaration *d)
{
    while (d->next != NULL) {
        d = d->next;
    }
    d->next = NEW (declaration);
    d->next->prev = d;
    d = d->next;
    return d;
}



 void   mergeAttributs( umlclassnode * classDest, umlclassnode * classACopier ){

    //////// MERGE ATTRIBUTS /////////
    umlattrlist listeDest = classDest->key->attributes;
    umlattrlist listACopier = classACopier->key->attributes;

//    printf ( "\n    ----- mergeAttributs ------------\n" );
    while ( listACopier != NULL ) {
        int bCopier = 1;
 //       printf ( "      -----> on cherche  %s?\n",   listACopier->key.name );
        umlattrlist listDestTemp = listeDest;
        // on compare les attributs a copier
        while ( listDestTemp != NULL ) {
//            printf ( "          ----->  %s?\n",  listDestTemp->key.name  );
             //  on compare les noms des attributs
            if (  eq ( listDestTemp->key.name , listACopier->key.name ) ){
                bCopier = 0;
            }

            listDestTemp = listDestTemp->next;
        }
        if ( bCopier )
        {
//            printf( "               -----> on copie : %s dans la dest.\n" ,listACopier->key.name );

            umlattrlist nodeCopy;
            nodeCopy = NEW (umlattrnode);
            nodeCopy->key = listACopier->key;
            nodeCopy->next = classDest->key->attributes;
            classDest->key->attributes = nodeCopy;
        }
//      else      printf( "               -----> attribut deja présent, on continue...\n" );

        listACopier = listACopier->next;

    }
//    printf ( "  ----- FIN mergeAttributs ------------\n\n" );


}


 void   mergeOperations( umlclassnode * classDest, umlclassnode * classACopier ){

    //////// MERGE OPERATIONS /////////
    umloplist listeOpDest = classDest->key->operations;
    umloplist listOpACopier = classACopier->key->operations;

  //  printf ( "\n    ----- mergeOperations ------------\n" );
    while ( listOpACopier != NULL ) {
        int bCopier = 1;
   //     printf ( "      -----> on cherche  %s?\n",   listOpACopier->key.attr.name );
        umloplist listDestTemp = listeOpDest;
        // on compare les attributs a copier
        while ( listDestTemp != NULL ) {
   //         printf ( "          ----->  %s?\n",  listDestTemp->key.attr.name );
            //  on compare les noms des attributs
            if (  eq ( listDestTemp->key.attr.name , listOpACopier->key.attr.name ) ){
                bCopier = 0;
            }

            listDestTemp = listDestTemp->next;
        }
        if ( bCopier )
        {
   //         printf( "               -----> on copie : %s dans la dest.\n" ,listOpACopier->key.attr.name );

            umloplist nodeCopy;
            nodeCopy = NEW (umlopnode);
            nodeCopy->key = listOpACopier->key;
            nodeCopy->next = classDest->key->operations;
            classDest->key->operations = nodeCopy;


        }
      //   else   printf( "               -----> attribut deja présent, on continue...\n" );

        listOpACopier = listOpACopier->next;

    }
//printf ( "  ----- FIN mergeOperations ------------\n\n" );

 }

 void   mergeAssociations( umlclassnode * classDest, umlclassnode * classACopier ){

    //////// MERGE OPERATIONS /////////
    umlassoclist listeOpDest = classDest->associations;
    umlassoclist listOpACopier = classACopier->associations;

    while ( listOpACopier != NULL ) {
        int bCopier = 1;

        umlassoclist listDestTemp = listeOpDest;
        // on compare les attributs a copier
        while ( listDestTemp != NULL ) {

            //  on compare les noms des attributs
            if (  eq ( listDestTemp->name , listOpACopier->name ) ){
                bCopier = 0;
            }

            listDestTemp = listDestTemp->next;
        }
        if ( bCopier )
        {

            umlassoclist tmp;
            tmp = NEW (umlassocnode);
            if (listOpACopier->name != NULL)
                sscanf(listOpACopier->name, "%s"/*sscanfmt()*/, tmp->name);

            if (listOpACopier->multiplicity != NULL)
                sscanf(listOpACopier->multiplicity, "%s", tmp->multiplicity);
            else
                sprintf(tmp->multiplicity, "1");

            tmp->key = listOpACopier->key;
            tmp->composite = listOpACopier->composite;
            tmp->visibility = listOpACopier->visibility;
           // tmp->multiplicity = listOpACopier->multiplicity;

            tmp->next = classDest->associations;
            classDest->associations = tmp;

        }

        listOpACopier = listOpACopier->next;

    }

 }
void
push (umlclassnode *node, batch *b)
{
    umlclasslist used_classes, tmpnode;
    module *m;
    declaration *d;
    namelist l_tmp;

    if (node == NULL ) {
        return;
    }

    declaration * dClassExistant = find_class (node);
    if ( dClassExistant != NULL) {
        // j'ajoute les attributs, operations, assoc...

        mergeAttributs ( dClassExistant->u.this_class , node );
        mergeOperations ( dClassExistant->u.this_class , node );
        mergeAssociations ( dClassExistant->u.this_class , node );

        return;
    }





    l_tmp = NEW (namenode);
    l_tmp->name = strdup (node->key->name);
    l_tmp->next = tmp_classes;
    tmp_classes = l_tmp;

    used_classes = list_classes (node, b);
    /* Make sure all classes that this one depends on are already pushed. */
    tmpnode = used_classes;
    while (tmpnode != NULL) {
        /* don't push this class !*/
        if (! eq (node->key->name, tmpnode->key->name) &&
            ! (is_present (tmp_classes, tmpnode->key->name) ^ b->mask)) {
            push (tmpnode, b);
        }
        tmpnode = tmpnode->next;
    }

    if (node->key->package != NULL) {
        umlpackagelist pkglist = make_package_list (node->key->package);
        m = find_or_add_module (&decls, pkglist);
        if (m->contents == NULL) {
            m->contents = NEW (declaration);
            d = m->contents;
            d->prev = NULL;
        } else {
            /* We can simply append because all classes that we depend on
               are already pushed. */
            d = append_decl (m->contents);
        }
    } else {
        if (decls == NULL) {
            decls = NEW (declaration);
            d = decls;
            d->prev = NULL;
        } else {
            d = append_decl (decls);
            /* We can simply append because all classes that we depend on
               are already pushed. */
        }
    }
    d->decl_kind = dk_class;
    d->next = NULL;
    d->u.this_class = NEW (umlclassnode);
    memcpy (d->u.this_class, node, sizeof(umlclassnode));
    if (strncmp (node->key->stereotype, "CORBA", 5) == 0)
        use_corba = 1;
}


