#include <Ennemies.h>
#include "../AcademiaEngine-Study/src/Engine/AcademiaEngine.h"

/*Voici comment corriger le mouvement pour que l'ennemi aille réellement vers le joueur — résumé et exemple concret à appliquer :
1.	Ne pas utiliser la position absolue du joueur comme "direction". Calculer le vecteur direction = playerPos - Position.
2.	Gérer le cas où il n'y a pas de Player (ne rien faire si player == nullptr).
3.	Normaliser le vecteur direction (division par sa longueur) pour obtenir une direction unitaire.
4.	Appliquer la force le long de ce vecteur unitaire : Position += direction_normalisée * force * elapsedTime.
5.	Optionnel : limiter la vitesse maximale, ajouter friction/accélération si besoin.
Exemple de code à remplacer dans Ennemies::Update (logique à coller telle quelle) :
•	vérifier GetPlayer() != nullptr
•	calculer playerPos et dir = playerPos - Position
•	float dist = sqrt(dir.xdir.x + dir.ydir.y);
•	if (dist < epsilon) return;
•	dir /= dist;
•	GoToPlayer(engine, force, dir, elapsedTime);
Remarques pratiques :
•	Ne change pas l'API si tu préfères : GoToPlayer accepte déjà un olc::vf2d "direction", donc transmettre dir normalisé fonctionne.
•	Évite que GetPlayerPosition retourne Position comme fallback (cela provoquait des multiplications illogiques) — mieux : retourne explicitement le playerPos ou fais la vérification de player dans Update.
•	Pour mouvement fluide et contrôlable, préfère stocker une vélocité (olc::vf2d velocity) et mettre Position += velocity * elapsedTime, en modifiant velocity par AddForce.*/

void Ennemies::Draw(AcademiaEngine& engine) {
	olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);
	engine.FillCircle(pixelPos, static_cast<int32_t>(Radius), Color);
}

const olc::vf2d& Ennemies::GetPlayerPosition(AcademiaEngine& engine) const {
	const Player* playerPtr = GetPlayer();
	if (playerPtr)
		return playerPtr->GetPosition();

	// Fallback: return this enemy's position if no player is set
	return Position;
}

void Ennemies::GoToPlayer(AcademiaEngine& engine, float force, olc::vf2d playerPosition, float elapsedTime) {
	AddForce(engine, force, playerPosition, elapsedTime);
}

void Ennemies::AddForce(AcademiaEngine& engine, float force, olc::vf2d direction, float elapsedTime)
{
	Position += direction * force * elapsedTime;
}

void Ennemies::Update(AcademiaEngine& engine, float elapsedTime)
{
	GoToPlayer(engine, 50.0f, GetPlayerPosition(engine), elapsedTime);
}
