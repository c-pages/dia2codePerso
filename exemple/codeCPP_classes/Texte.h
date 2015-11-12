#ifndef TEXTE__H
#define TEXTE__H

/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include "Gadget.h"


/////////////////////////////////////////////////
/// \brief 
///
/////////////////////////////////////////////////
class Texte : public Gadget {
    
    /////////////////////////////////////////////////
    // Les methodes
    /////////////////////////////////////////////////
    
public:
Texte (ptr parent);

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /// \param parent		 
    /// \param texte		 
    /////////////////////////////////////////////////
Texte (ptr parent, string texte);

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /// \param texte		 
    /////////////////////////////////////////////////
void setTexte (string texte);

virtual void init ();

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /////////////////////////////////////////////////
virtual void init_skin ();

virtual void actualiser ();

virtual void actualiser_bounds ();

    
    /////////////////////////////////////////////////
    // Les membres
    /////////////////////////////////////////////////
    
private:
    sf::RectangleShape * m_rectangle;    
    sf::Texture m_texture;    ///<       
};

#endif


////////////////////////////////////////////////////////////
/// class Texte
/// ingroup 
///
/// see 
///
////////////////////////////////////////////////////////////