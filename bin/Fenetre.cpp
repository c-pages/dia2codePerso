/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include <Fenetre.h>


/////////////////////////////////////////////////
// Initialisation des statics
/////////////////////////////////////////////////
titi  Fenetre::popo = 5.
GUI  Fenetre::ms_gui = make_shared<GUI> (new GUI()).


/////////////////////////////////////////////////
Fenetre::Fenetre (ptr parent)
: m_deplacable ( true )
, m_redimensionnable ( true )
, m_fermable ( true )
, m_deplacement ( false )
, m_redim_D ( false )
, m_redim_G ( false )
, m_redim_H ( false )
, m_redim_B ( false )
, m_fenetrePosition_debut (  )
, m_sourisPosition_debut (  )
, m_fenetreTaille_debut (  )
, m_lblTitre ( new () )
, m_btnDrag ( new () )
, m_btnFermer ( new () )
, m_btnRedim_BD ( new () )
, m_btnRedim_BG ( new () )
, m_btnRedim_HD ( new () )
, m_btnRedim_HG ( new () )
, m_btnRedim_G ( new () )
, m_btnRedim_D ( new () )
, m_btnRedim_B ( new () )
, m_btnRedim_H ( new () )
, m_contenuCadre ( new () )
, m_contenu ( new () )
, m_fond ( new () )
{
    
}


/////////////////////////////////////////////////
Fenetre::Fenetre (ptr parent, string titre, bool redim, bool fermable, bool deplacable)
: m_deplacable ( true )
, m_redimensionnable ( true )
, m_fermable ( true )
, m_deplacement ( false )
, m_redim_D ( false )
, m_redim_G ( false )
, m_redim_H ( false )
, m_redim_B ( false )
, m_fenetrePosition_debut (  )
, m_sourisPosition_debut (  )
, m_fenetreTaille_debut (  )
, m_lblTitre ( new () )
, m_btnDrag ( new () )
, m_btnFermer ( new () )
, m_btnRedim_BD ( new () )
, m_btnRedim_BG ( new () )
, m_btnRedim_HD ( new () )
, m_btnRedim_HG ( new () )
, m_btnRedim_G ( new () )
, m_btnRedim_D ( new () )
, m_btnRedim_B ( new () )
, m_btnRedim_H ( new () )
, m_contenuCadre ( new () )
, m_contenu ( new () )
, m_fond ( new () )
{
    
}


/////////////////////////////////////////////////
Fenetre::setTitre (string titre)
{
    
}


/////////////////////////////////////////////////
void Fenetre::ajouter (ptr enfant)
{
    
}


/////////////////////////////////////////////////
void Fenetre::supprimer (ptr enfant)
{
    
}


/////////////////////////////////////////////////
void Fenetre::init ()
{
    
}


/////////////////////////////////////////////////
void Fenetre::actualiser ()
{
    
}


/////////////////////////////////////////////////
void Fenetre::traiter_evenements (const sf::Event& evenement)
{
    
}


/////////////////////////////////////////////////
void Fenetre::init_skin ()
{
    
}


/////////////////////////////////////////////////
void Fenetre::init_interactions ()
{
    
}


/////////////////////////////////////////////////
void Fenetre::actualiser_bounds ()
{
    
}


