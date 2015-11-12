#ifndef LOG__H
#define LOG__H

/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include "Action.h"
#include "Gadget.h"


/////////////////////////////////////////////////
/// \brief 
///
/////////////////////////////////////////////////
class Log : public Action, public Gadget {
    
    /////////////////////////////////////////////////
    // Les methodes
    /////////////////////////////////////////////////
    
public:
    /////////////////////////////////////////////////
    /// \brief  
    ///
    /////////////////////////////////////////////////
Log ();

void setTexte (string texte);

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /////////////////////////////////////////////////
static void print ();

virtual void init ();

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /////////////////////////////////////////////////
virtual void init_skin ();

virtual void actualiser ();

virtual virtual void traiter_evenement (const sf::Event& evenement);

    
    /////////////////////////////////////////////////
    // Les membres
    /////////////////////////////////////////////////
    
public:
    std::string m_preligne;    ///<       
private:
    std::shared_ptr<Texte> m_lblMouseX;    
    shared_ptr<Texte> m_lblMouseY;    
    shared_ptr<Texte> m_lblFPS;    
};

#endif


////////////////////////////////////////////////////////////
/// class Log
/// ingroup 
///
/// see 
///
////////////////////////////////////////////////////////////