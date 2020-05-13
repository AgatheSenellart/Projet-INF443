#pragma once
#include "vcl/math/transformation/affine_transform/affine_transform.hpp"
#include "vcl/math/transformation/special_transform/special_transform.hpp"
#include "vcl/shape/mesh/mesh_drawable/mesh_drawable.hpp"


namespace vcl {
	struct noeud
	{
		// longueur de la branche. Elle vaut 0 si c'est un bourgeon
		float length;
		noeud* fils1 = 0;
		noeud* fils2 = 0;
		noeud* fils3 = 0;
		noeud* fils4 = 0;
		// qui représente la transformation globale de l'élément
		affine_transform T;


	};

	
	

	noeud* new_bourgeon();
	noeud* new_branche(int length);
	mesh_drawable branche(float taille_branche);

	mesh_drawable feuille(float taille_branche);

	// Create a tree using the grammar 
	noeud* grammar_tree(int level);
	
	void apply_grammar(noeud* n);
	

	void fill_coordinates(noeud* n, float taille_branche);

	void draw(const noeud* arbre, const camera_scene& camera, mesh_drawable branche,mesh_drawable feuille, vec3 position);


}



