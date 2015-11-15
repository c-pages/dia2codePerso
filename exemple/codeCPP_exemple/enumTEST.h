#ifndef ENUMTEST__H
#define ENUMTEST__H

/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////


namespace gui {

/////////////////////////////////////////////////
/// \brief les evenements de la souris qui déclenchent une fonction lambda associée.
///
/////////////////////////////////////////////////
enum enumTEST {
    onBtnG_DblClique,        ///< Double clique gauche
    onBtnG_Relache,        ///< Relache le bouton gauche de la souris.
    onBtnG_RelacheDehors,        ///< Relache a coté du gadget avec le bouton gauche de la souris après avoir pressé dedans.
    onBtnG_Press,        ///< Presse le bouton gauche de la souris.
    onBtnG_PressDehors,        ///< Presse a coté du gadget avec le bouton gauche de la souris.
    onBtnD_Relache,        ///< Relache le bouton droit de la souris.
    onBtnD_RelacheDehors,        ///< Relache le bouton droit de la souris.
    onBtnD_Press,        ///< Presse le bouton droit de la souris.
    onBtnM_Relache,        ///< Relache le bouton du milieu de la souris.
    onBtnM_RelacheDehors,        ///< Relache le bouton du milieu de la souris.
    onBtnM_Press,        ///< Presse le bouton du milieu de la souris.
    onBtnM_RollUp,        ///< Roule vers le haut le bouton du milieu de la souris.
    onBtnM_RollDown,        ///< Roule vers le bas le bouton du milieu de la souris.
    onBtn_Entre,        ///< Survol
    onBtn_Sort,        ///< Quitte le survol.
    onBtn_changeEtat,        ///< quand BoutonACocher change d'état
    onFen_Ferme,        ///< Quand on ferme une fenetre.
    onFen_Ouvre,        ///< Quand on ferme une fenetre.
    onCha_ChangeValeur,        ///< Quand on change la valeur d'un champ de saisie de texte.
    onCha_ValideValeur        ///< Quand on valide la valeur d'un champ de saisie de texte.
};

} // fin namespace gui

#endif
