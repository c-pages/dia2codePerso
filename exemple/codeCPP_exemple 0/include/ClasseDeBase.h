#ifndef CLASSEDEBASE__H
#define CLASSEDEBASE__H

/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include "ClasseC.h"
#include "ClasseA.h"
#include <memory>
#include "ClasseB.h"





/////////////////////////////////////////////////
/// \brief Classe qui h�rite de 2 autres classes. Cela ajoutera les includes nec�ssaires.
///
/////////////////////////////////////////////////
class ClasseDeBase : public ClasseC, public ClasseA {



/////////////////////////////////////////////////
// M�thodes
/////////////////////////////////////////////////

public:
    ///< Definir m_composition
    void setComposition( ClasseB val ){ m_composition = val; };

    ///< Acceder � m_composition
    ClasseB getComposition () const { return m_composition; };

    ///< Definir m_aggregation
    void setAggregation( std::shared_ptr<ClasseB> val ){ m_aggregation = val; };

    ///< Acceder � m_aggregation
    std::shared_ptr<ClasseB> getAggregation () const { return m_aggregation; };

public:
    /////////////////////////////////////////////////
    /// \brief Constructeur, les attributs ayant une valeur par d�faut vont �tre initialis�s dans le *.cpp.
    ///
    /////////////////////////////////////////////////
    ClasseDeBase ();

    /////////////////////////////////////////////////
    /// \brief operation public de base.
    ///
    /// \param param1		 Parametre de l'operation.
    /////////////////////////////////////////////////
    void operation_1 (int param1);

    /////////////////////////////////////////////////
    /// \brief operation virtuelle qui override celle de la ClasseA.
    ///
    /////////////////////////////////////////////////
    virtual void operation_virtuelle ();

    /////////////////////////////////////////////////
    /// \brief operation virtuelle qui override l'operation abstraite de la ClasseA.
    ///
    /////////////////////////////////////////////////
    virtual void operation_abstraite ();



/////////////////////////////////////////////////
// Membres
/////////////////////////////////////////////////
public:
    static int ms_attr_1;    ///< Attribut static de la classe de base. Sera initialis� au d�but du *.cpp.    
    int m_attr_2;    ///< Attribut public de la classe de base.    
protected:
    int m_attr_3;    ///< Attribut proteg� de la classe de base.    
    ClasseB m_composition;     ///< associ� avec le type: 'composition', cr�er un simple attribut. #G#S
    std::shared_ptr<ClasseB> m_aggregation;     ///< associ� avec le type: 'Aggregation'. Cr�er l'attribut dans un shared_ptr. En mettant #G ou #S dans le commentaire d'un attribut on indique a dia2code  de prevoir les fonctions get ou set dans le *.h.

}; // fin class ClasseDeBase

#endif
