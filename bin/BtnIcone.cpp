/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include <BtnIcone.h>

/////////////////////////////////////////////////
BtnIcone::BtnIcone (ptr parent)
: m_etat ( repos )
, m_clock_dblClique ( new Clock() )
, m_1erClick ( false )
, m_label ( new () )
, m_icone ( new () )
, m_bouton ( new () )
{
    
}


/////////////////////////////////////////////////
BtnIcone::BtnIcone (ptr parent, string texte)
: m_etat ( repos )
, m_clock_dblClique ( new Clock() )
, m_1erClick ( false )
, m_label ( new () )
, m_icone ( new () )
, m_bouton ( new () )
{
    
}


/////////////////////////////////////////////////
BtnIcone::BtnIcone (ptr parent, sf::Texture icone)
: m_etat ( repos )
, m_clock_dblClique ( new Clock() )
, m_1erClick ( false )
, m_label ( new () )
, m_icone ( new () )
, m_bouton ( new () )
{
    
}


/////////////////////////////////////////////////
void BtnIcone::setEtat (Etat etat)
{
    
}


/////////////////////////////////////////////////
void BtnIcone::setTexte (std::string texte)
{
    
}


/////////////////////////////////////////////////
void BtnIcone::setIcone (const sf::Texture* icone)
{
    
}


/////////////////////////////////////////////////
virtual void BtnIcone::actualiser ()
{
    
}


/////////////////////////////////////////////////
virtual void BtnIcone::traiter_evenements (const sf::Event& evenement)
{
    
}


/////////////////////////////////////////////////
virtual void BtnIcone::init ()
{
    
}


/////////////////////////////////////////////////
virtual void BtnIcone::init_skin ()
{
    
}


/////////////////////////////////////////////////
virtual void BtnIcone::init_interactions ()
{
    
}


/////////////////////////////////////////////////
virtual void BtnIcone::actualiser_bounds ()
{
    
}


