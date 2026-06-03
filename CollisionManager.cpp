#include "CollisionManager.h"
#include <algorithm>
#include <cmath>
#include "Player.h"
#include "Ennemies.h"

void CollisionManager::RegisterCollider(Collider* collider)
{
	// Implementation for registering a collider
	if (!collider) {
		return; // Handle null pointer case
	}
	colliders.push_back(collider);
}

void CollisionManager::UnregisterCollider(Collider* collider)
{
	// Implementation for unregistering a collider
	if (!collider) {
		return; // Handle null pointer case
	}
	auto it = std::remove(colliders.begin(), colliders.end(), collider);
	if (it != colliders.end()) {
		colliders.erase(it, colliders.end());
	}
}

void CollisionManager::Update()
{
	// Implementation for updating collisions
	for (int i = 0; i < colliders.size(); ++i) {
		for (int j = i + 1; j < colliders.size(); ++j) {
			Collider* colliderA = colliders[i];
			Collider* colliderB = colliders[j];

			// Check if both colliders are enabled
			if (colliderA->enabled && colliderB->enabled) {
				const bool collisionMatrix[4][4] = {
					// 0    1      2      3
					{false, false, false, false}, // 0 unused
					{false, false, true,  false}, // 1 Player
					{false, true,  false, true }, // 2 Enemy
					{false, false, true,  false}  // 3 Bullet
				};

				int a = colliderA->layer;
				int b = colliderB->layer;
				std::cout << "Checking collision between layer " << a << " and layer " << b << std::endl;
				if (collisionMatrix[a][b]){
					std::cout << "Collision detected between layer " << a << " and layer " << b << std::endl;
					// Collision logic
					if (colliderA->type == Collider::EColliderType::Circle && colliderB->type == Collider::EColliderType::Circle) {
						// Circle vs Circle collision logic
						olc::vf2d delta = colliderB->position - colliderA->position;
						float distanceSquared = delta.mag2();
						float radiusSum = colliderA->size + colliderB->size;
						if (distanceSquared < radiusSum * radiusSum) {
							// Handle collision response here
							if (collisionMatrix[1][2] || collisionMatrix[2][1]) { // Player-Enemy collision
								if (player->damageCooldown <= 0.0f) { // Check if player can take damage)
									player->TakeDamage(10.0f); // Example damage value
									player->damageCooldown = player->damageDelay; // reset cooldown
									std::cout << "Player took damage! Current health: " << player->GetHealth() << std::endl;
								}
							}
							if (collisionMatrix[2][3] || collisionMatrix[3][2]) { // Enemy-Bullet collision
								if (colliderA->layer == 2) { // colliderA is Enemy
									static_cast<Ennemies*>(colliderA->owner)->TakeDamage(25.0f);
									std::cout << "Enemy took damage! Current health: ";
								}
								else { // colliderB is Enemy
									static_cast<Ennemies*>(colliderB->owner)->TakeDamage(25.0f);
									std::cout << "Enemy took damage! Current health: ";
								}
							}
						}
					}
					if (colliderA->type == Collider::EColliderType::Box && colliderB->type == Collider::EColliderType::Box) {
						// Box vs Box collision logic
						olc::vf2d delta = colliderB->position - colliderA->position;
						if (std::abs(delta.x) < (colliderA->size + colliderB->size) &&
							std::abs(delta.y) < (colliderA->size + colliderB->size)) {
							// Handle collision response here

						}
					}
					if ((colliderA->type == Collider::EColliderType::Circle && colliderB->type == Collider::EColliderType::Box) ||
						(colliderA->type == Collider::EColliderType::Box && colliderB->type == Collider::EColliderType::Circle)) {
						// Circle vs Box collision logic
						Collider* circleCollider = (colliderA->type == Collider::EColliderType::Circle) ? colliderA : colliderB;
						Collider* boxCollider = (colliderA->type == Collider::EColliderType::Box) ? colliderA : colliderB;
						olc::vf2d delta = circleCollider->position - boxCollider->position;
						olc::vf2d halfSize = { boxCollider->size, boxCollider->size };
						olc::vf2d closestPoint = delta.clamp(-halfSize, halfSize); //Only for square box, for rectangle we would need to have different halfSize for x and y
						olc::vf2d difference = delta - closestPoint;
						float distanceSquared = difference.mag2();
						if (distanceSquared < circleCollider->size * circleCollider->size) {
							// Handle collision response here

						}
					}
				}
			}
		}
	}
}

/*Pistes principales et actions rapides à tester (court et concret)
1.	Vérifier que Update est bien appelé
•	Ajoute un log/breakpoint au tout début de AcademiaEngineOnUserUpdate et au début de GameManagerUpdate pour confirmer que la boucle arrive jusque là.
2.	Crash / accès mémoire (arrêt prématuré)
•	Symptôme : programme se ferme/disparaît sans poursuivre.
•	Cause fréquente : pointeur invalide (collider->owner null ou dangling), accès à player non initialisé, static_cast sur pointeur corrompu.
•	Test : avant d’utiliser owner ou player, ajoute des guards : if (!colliderA || !colliderB) continue; if (!colliderA->owner || !colliderB->owner) continue; if (!player) continue;
•	Exécute sous le débogueur (Break on All Exceptions / Access Violation) pour voir la pile.
3.	Blocage / boucle longue
•	Symptôme : la frame freeze (ne progresse pas).
•	Cause : boucle coûteuse (double boucle sur beaucoup d'objets), appel bloquant (mutex, sleep) ou timer mal utilisé.
•	Test : temporise / mesure temps d'exécution : auto t0 = stdchronohigh_resolution_clocknow(); _CollisionManager.Update(); auto t1 = stdchronohigh_resolution_clocknow(); log durée (t1-t0) pour détecter hotspot.
4.	Invalidation d'itérateurs (modifs pendant Update)
•	Symptôme : crash sporadique pendant _CollisionManager.Update.
•	Cause : Register/Unregister appelés depuis un callback pendant l'itération.
•	Test : protège Register/Unregister en différant les ajouts/suppressions (pending vectors) ou empêche les callbacks de modifier la collection pendant la passe.
5.	Early return logique
•	Symptôme : Update s’exécute mais s’arrête tôt (retours conditionnels).
•	Test : place des logs/printf à plusieurs points (début, après _CollisionManager.Update, après boucle bullets/ennemies) pour localiser l'arrêt logique.
6.	Assertions / exceptions C++
•	Active «Break on C++ exceptions» et regarde la console Output pour exceptions non interceptées.
Checklist d’actions immédiates (exécuter maintenant)
•	Met un breakpoint au début de GameManager::Update.
•	Si breakpoint sauté -> vérifier OnUserUpdate est appelé.
•	Si breakpoint atteint puis pas plus loin -> placer logs successifs dans Update pour localiser la passerelle qui bloque/crashe.
•	Lancer en Debug, quand ça plante consulter la call stack.
Si tu veux, j’ajoute :
•	des logs temporaires dans GameManagerUpdate et CollisionManagerUpdate,
•	ou les guards rapides (null checks) dans CollisionManager.Update que je peux appliquer immédiatement. Quelle option ?*/