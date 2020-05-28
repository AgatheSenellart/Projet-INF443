#include "vcl/math/transformation/affine_transform/affine_transform.hpp"
#include "vcl/math/transformation/special_transform/special_transform.hpp"
#include "vcl/shape/mesh/mesh_drawable/mesh_drawable.hpp"
#include "vcl/shape/mesh/mesh_primitive/mesh_primitive.hpp"
#include "tree.hpp"

namespace vcl {

	
	

	noeud* new_bourgeon() {
		noeud* bourgeon = new noeud;
		bourgeon->length = 0;
		bourgeon->T = affine_transform();
		return bourgeon;
	}

	noeud* new_branche(int length)
	{
		noeud* bourgeon = new noeud;
		bourgeon->length = length;
		bourgeon->fils1 = new_bourgeon();
		bourgeon->fils2 = new_bourgeon();
		bourgeon->fils3 = new_bourgeon();
		bourgeon->fils4 = new_bourgeon();
		bourgeon->T = affine_transform();
		return bourgeon;
	}


	void apply_grammar(noeud* n)
	{
		

		if (n == nullptr) return;
        // Si c'est un bourgeon ca devient une branche
		if (n->length == 0)
		{
			n->length =1.0f;
			// on ajoute trois nouveaux bourgeons
			n->fils1 = new_bourgeon();
			n->fils2 = new_bourgeon();
			n->fils3 = new_bourgeon();
			n->fils4 = new_bourgeon();
			return;
		}
        // Si c est une branche on double sa longueur et on recurse
		if (n->length != 0)
		{
			n->length = 1.7f * n->length;
			apply_grammar(n->fils1);
			apply_grammar(n->fils2);
			apply_grammar(n->fils3);
			apply_grammar(n->fils4);
		}
	}

	void fill_coordinates(noeud* n,float taille_branche)
    {	// Dans cette fonction on remplit les coordonnées T de chaque noeud de l arbre
        // on suppose la matrice T de la racine comme renseignee
		if (n == 0) return;
		if (n->length == 0) return;
		// matrice de dilatation à faire avant la matrice T
        n->length = n->length * rand_interval(0.6f, 1.5f);
		// angle 
		float alpha = 3.14f / 4.0f;
		// On définit les matrices de transformations locales
		// pour la première branche en bas
		vcl::affine_transform T1;
		T1.translation = { 0,0,taille_branche * n->length* rand_interval(0.5f,0.8f)};
		T1.rotation = rotation_between_vector_mat3(vec3(0, 0, 1.0f), vec3(sin(0.40), 0, cos(0.40)));
		// Pour la deuxième branche 
		vcl::affine_transform T4;
		T4.translation = { 0,0,taille_branche * n->length *rand_interval(0.6f,0.9f) };
		T4.rotation = rotation_between_vector_mat3(vec3(0, 0, 1.0f), vec3(sin(0.52)*cos(2*3.14f/2), sin(0.52)*sin(2*3.14f/2), cos(0.52)));
		// Pour les deux branches en haut
		vcl::affine_transform T2;
		T2.translation = { 0,0,taille_branche*n->length };
		T2.rotation = rotation_between_vector_mat3(vec3(0, 0, 0.9f), vec3(sin(0.82) * cos( 3.14f / 2.0f), sin(0.82) * sin( 3.14f / 2.0f), cos(0.82)));

		vcl::affine_transform T3;
		T3.translation = { 0,0,taille_branche * n->length };
		T3.rotation = rotation_between_vector_mat3(vec3(0, 0, 1.0f), vec3(sin(0.65) * cos(3 * 3.14f / 2.0f), sin(0.65) * sin(3 * 3.14f / 2.0f), cos(0.65)));
		//Une rotation aléatoire en plus autour de l'axe z
		vcl::affine_transform R;
		R.rotation = rotation_from_axis_angle_mat3({ 0,0,1 }, rand_interval(0.0f, 2 * 3.14f));

		// on remplit les matrices T des fils
		n->fils1->T = (n->T) *R* T1;
		n->fils3->T = (n->T) *R* T3; 
		n->fils2->T = (n->T) *R* T2;
		n->fils4->T = (n->T) *R* T4;

		//affine_transform dilatation;
		affine_transform dilatation;
		float ratio = rand_interval(0.9f, 1.5f);
		dilatation.scaling_axis = vec3(n->length * ratio, n->length * ratio, n->length);
		n->T = (n->T) * dilatation;
		

		//Récursion
		fill_coordinates(n->fils1,taille_branche); fill_coordinates(n->fils2,taille_branche); fill_coordinates(n->fils3,taille_branche); fill_coordinates(n->fils4,taille_branche);



	}
	mesh_drawable branche(float taille_branche)
	{
		
		mesh branche_cpu = mesh_primitive_cylinder(0.03f * taille_branche, { 0,0,0 }, { 0,0,taille_branche });
		mesh haut_branche = mesh_primitive_sphere(0.03f*taille_branche, { 0,0,taille_branche });
		branche_cpu.push_back(haut_branche);
		mesh_drawable branche = branche_cpu;
		branche.uniform.color = { 0.4f,0.2f, 0.0f };
		return branche;
		//attention les shaders ne sont pas définis
	}

	mesh_drawable feuille(float taille_branche)
	{
		mesh feuille_cpu;
		float taille = 0.35f*taille_branche;
		feuille_cpu.position = { {taille* -0.15f,taille* -0.5f,taille}, {0,0,2.0f*taille}, {-0.15f*taille,0.5f*taille,taille}, {0,0,0.0f} };
		feuille_cpu.connectivity = { {0,1,2},{2,3,0} };
		feuille_cpu.normal = { {1,-0.05f,0},{1, 0.05f,0} };
		mesh_drawable feuille = feuille_cpu;
		feuille.uniform.color = { 0.2f,0.4f,0.0f };
		return feuille;
	}

	// Create a tree using the grammar 
	noeud* grammar_tree(int level)
	{
		noeud* basic(0);
		basic = new_bourgeon();
		for (int i = 0; i < level; i++)
		{
			apply_grammar(basic);
		}
		return basic;
	}

	void draw(const noeud* arbre, const camera_scene& camera, mesh_drawable branche, mesh_drawable feuille, vec3 position)
	{
		if (arbre == 0) { return; }
		
		// on déclare l'élement visuel à dessiner
		mesh_drawable visual_element;
		// la position de l'arbre
		affine_transform translation;
		translation.translation = position;

		if (arbre->length == 0)
		{
			visual_element = feuille;
			visual_element.uniform.transform = translation * (arbre->T);
		}
		else
		{
			visual_element = branche;
			visual_element.uniform.transform = translation * (arbre->T);

		}

		vcl::draw(visual_element, camera);
		if (arbre->length == 0) return;
		draw(arbre->fils1, camera, branche,feuille,position);
		draw(arbre->fils2, camera, branche,feuille,position);
		draw(arbre->fils3, camera, branche,feuille,position);
		draw(arbre->fils4, camera, branche,feuille,position);
	}


}
