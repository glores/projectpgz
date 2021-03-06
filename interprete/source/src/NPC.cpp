/*----------------------------------------------------------------------------------*/
//This code is part of a larger project whose main purpose is to entretain either	//
//by working on it and helping it be better or by playing it in it's actual state	//																			
//																					//
//Copyright (C) 2011 Three Legged Studio											//
//																					//
//    This program is free software; you can redistribute it and/or modify			//
//    it under the terms of the GNU General Public License as published by			//
//    the Free Software Foundation; either version 2, or (at your option)			//
//    any later version.															//
//																					//
//    This program is distributed in the hope that it will be useful,				//
//    but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the					//
//    GNU General Public License for more details.									//
//																					//
//    You should have received a copy of the GNU General Public License				//
//    along with this program; if not, write to the Free Software Foundation,		//
//    Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.			//
//																					//
//	  You can contact us at projectpgz.blogspot.com									//
/*----------------------------------------------------------------------------------*/

#include "NPC.h"

NPC::NPC(int x, int y, Game* game, GameState* world) : GameEntity(x, y, game, world) {
	mask = new MaskBox(x, y, 12, 8, "npc", 2, 14); 
	solid = true;
	type = "npc";
	flag = false;
}

NPC::~NPC() {
	animList.clear();
	animDataList.clear();
}

void NPC::init(string graphicpath, int type, int textId, Controller* c, Direction d) {
	ox = x;
	oy = y;
	sp = 2;
	state = move;
	t = (Type)type;
	idText = textId;
	dir = DOWN;
	controller = c;
	this->setTimer(0, 60); 

	loadAnimations(graphicpath, getConfigurationFileName(graphicpath));
	initShadow(GameEntity::sSmall);

	//if (t == Type::oldMan)
	((SpriteMap*) graphic)->playAnim(getAnimName(Stand, dir));
}

void NPC::onStep(){
	if (GameEntity::isPaused()){ 
		/* Si el juego est� bloqueado no hacemos nada */
		return;
	}
	else if (state == interact){
		/* Si se acaba de desbloquear */
		onEndInteract();
	}

	if (state == move){
		if (t == Type::crazy){ /* Si el NPC no se mueve s�lo tiene que girar */
			((SpriteMap*) graphic)->playAnim(getAnimName(Stand, dir));	
		}
		else if (t == Type::young){
			switch (dir){
				case UP:
					if (place_free(x, y - sp)){
						if (ox - x == 16 || oy - y == 16 || ox - x == -16 || oy - y == -16){
							ox = x;
							oy = y;
							state = idle;
							((SpriteMap*) graphic)->playAnim(getAnimName(Stand, dir));
						}
						else{
							y -= sp;
							((SpriteMap*) graphic)->playAnim(getAnimName(Walk, dir));
						}
					}
					else{
						state = idle;
						((SpriteMap*) graphic)->playAnim(getAnimName(Stand, dir));
					}
					break;
				case DOWN:
					if (place_free(x, y + sp)){
						if (ox - x == 16 || oy - y == 16 || ox - x == -16 || oy - y == -16){
							ox = x;
							oy = y;
							state = idle;
							((SpriteMap*) graphic)->playAnim(getAnimName(Stand, dir));
						}
						else{
							y += sp;
							((SpriteMap*) graphic)->playAnim(getAnimName(Walk, dir));
						}
					}
					else{
						state = idle;
						((SpriteMap*) graphic)->playAnim(getAnimName(Stand, dir));
					}
					break;
				case LEFT:
					if (place_free(x - sp,y)){
						if (ox - x == 16 || oy - y == 16 || ox - x == -16 || oy - y == -16){
							ox = x;
							oy = y;
							state = idle;
							((SpriteMap*) graphic)->playAnim(getAnimName(Stand, dir));
						}
						else{
							x -= sp;
							((SpriteMap*) graphic)->playAnim(getAnimName(Walk, dir));
						}
					}
					else{
						state = idle;
						((SpriteMap*) graphic)->playAnim(getAnimName(Stand, dir));
					}
					break;
				case RIGHT:
					if (place_free(x + sp,y)){
						if (ox - x == 16 || oy - y == 16 || ox - x == -16 || oy - y == -16){
							ox = x;
							oy = y;
							state = idle;
							((SpriteMap*) graphic)->playAnim(getAnimName(Stand, dir));
						}
						else{
							x += sp;
							((SpriteMap*) graphic)->playAnim(getAnimName(Walk, dir));
						}
					}
					else{
						state = idle;
						((SpriteMap*) graphic)->playAnim(getAnimName(Stand, dir));
					}
					break;
			}
		}
	}

	if (mask != NULL)
		depth = y + mask->yoffset;
	else
		depth = y;
}

bool NPC::animFinished() {
	return ((SpriteMap*) graphic)->animFinished();
}

void NPC::onTimer(int timer){
	int op = rand() % 2;
	int d;
	switch (timer) {
		case 0: 
			// Si el NPC no est� en el estado interactuar
			if (state != interact){
				switch (op){
					case 0:
						state = idle;
						break;
					case 1:
						state = move;
						d = rand() % 4;
						switch (d){
							case 0: dir = UP;
								break;
							case 1: dir = DOWN;
								break;
							case 2: dir = LEFT;
								break;
							case 3: dir = RIGHT;
								break;
						}
						break;
				}
				break;
			}
	}
	this->setTimer(0, rand()%60);
}

void NPC::onInteract(Player* p) {
	state = interact;
	switch (p->dir)	{
		case DOWN: dir = UP; break;
		case UP: dir = DOWN; break;
		case LEFT: dir = RIGHT; break;
		case RIGHT: dir = LEFT; break;
	}
	if (graphic != NULL)
		((SpriteMap*) graphic)->playAnim(getAnimName(Stand, dir));
	// Inicia una conversaci�n con el player. MessageController volver� a restablecer el valor del flag cuando detecte que ha terminado
	flag = true; 
	this->controller->getMessageController()->showMessageDialog(idText, this);
}

void NPC::onEndInteract(){
	state = idle;
}

std::string NPC::getConfigurationFileName(string fname) {
	// El fname debe tener extensi�n
	if (fname.length() < 5)
		return "";

	std::string cfgname = fname;
	// Se quita la extensi�n del fname (.png)
	for (int i = 0; i < 3; i++)
		cfgname.pop_back();
	cfgname.append("cfg");

	return cfgname;
}

bool NPC::loadAnimations(string graphicpath, string fname) {
	SpriteMap* gfx = ((SpriteMap*) graphic);
	int nCols = 0, nRows = 0;

	// Carga el archivo de config y lee
	FILE* f = fopen(fname.c_str(), "rb");

	// Si el archivo es inv�lido, no se puede hacer nada
	if (f == NULL)
		return false;

	// 1. N�mero de columnas y filas del sprite sheet
	if (fscanf(f, "%d %d", &nCols, &nRows) < 2)
		return false;

	graphic = new SpriteMap(graphicpath, nCols, nRows, game->getGfxEngine());

	// Datos de la m�scara
	int mOx, mOy, mW, mH;

	// 1.5. Leer la m�scara
	if (fscanf(f, "%d %d %d %d", &mOx, &mOy, &mW, &mH) < 4)
		return false;

	if (mask != NULL)
	{
		mask->xoffset = mOx;
		mask->yoffset = mOy;
		mask->width = mW;
		mask->height = mH;
	}
	else
		mask = new MaskBox(x, y, mW, mH, "npc", mOx, mOy);

	// 2. Leer las animaciones
	// Stand
	loadAnimation(Stand, UP, "idleUp", f);
	loadAnimation(Stand, DOWN, "idleDown", f);
	loadAnimation(Stand, RIGHT, "idleRight", f);
	loadAnimation(Stand, LEFT, "idleLeft", f);

	// Walk
	if (t == Type::young){ /* Si el NPC anda, cargamos sus animaciones */
		loadAnimation(Walk, UP, "up", f);
		loadAnimation(Walk, DOWN, "down", f);
		loadAnimation(Walk, RIGHT, "right", f);
		loadAnimation(Walk, LEFT, "left", f);
	}

	fclose(f);

	return true;
}

bool NPC::loadAnimation(NPCAnim anim, Direction dir, std::string name, FILE* from) {
	// Cargar animaci�n indicada de from
	if (from == NULL || name == "")
		return false;

	int numFrames, speed;
	int* frameList;
	NPCAnimData animData;
	NPCFrameData frameData;

	// Leemos datos de la animaci�n

	// 0.Speed
	if (fscanf(from, "%d", &speed) < 1)
		return false;

	// 1.N� frames
	if (fscanf(from, "%d", &numFrames) < 1)
		return false;

	animData.animSpeed = speed;
	animData.numFrames = numFrames;
	//animData.dir = dir;

	// Se instancia el contenedor de frames
	frameList = new int[numFrames];

	// Se carga cada frame
	for (int i = 0; i < numFrames; i++) 	{
		frameData = loadAnimationFrame(from);
		frameList[i] = frameData.frameId;
		animData.frameData.push_back(frameData);
	}

	// Se a�ade la animaci�n al graphic con el nombre indicado
	SpriteMap* gfx = ((SpriteMap*) graphic);
	gfx->addAnim(name, frameList, numFrames, speed, true);

	// Se mappea la anim
	animList.insert(make_pair(make_pair(anim, dir), name));
	// Y se agrega su info
	animDataList.insert(make_pair(make_pair(anim, dir), animData));
	delete frameList;

	return true;
}

NPC::NPCFrameData NPC::loadAnimationFrame(FILE* from) {
	// Se carga un frame de from
	// Se prepara un frame vac�o
	NPCFrameData fd;
	fd.frameId = 0;
	fd.walkMask.offsetX = 0;
	fd.walkMask.offsetY = 0;
	fd.walkMask.width = 0;
	fd.walkMask.height = 0;
	fd.collisionMask.offsetX = 0;
	fd.collisionMask.offsetY = 0;
	fd.collisionMask.width = 0;
	fd.collisionMask.height = 0;

	// El archivo debe ser v�lido
	if (from == NULL)
		return fd;

	// Se lee el frameId
	if (fscanf(from, "%d", &fd.frameId) < 1)
		return fd;

	// Y por ahora ya
	return fd;
}

NPC::NPCAnimData NPC::getAnimationData(NPCAnim anim, Direction dir) {
	if (dir == NONE) dir = this->dir;
	// Se obtiene el nombre de la animaci�n a partir del enum
	std::map<std::pair<NPCAnim, Direction>, NPCAnimData>::iterator it;
	it = animDataList.find(make_pair(anim, dir));
	// Si el iterador alcanca el final de la lista, no est� la anim
	if (it == animDataList.end()) 	{
		NPCAnimData a; a.animSpeed = -1; a.numFrames = -1;
		return a;
	}
	else
		return (*it).second;
}

bool NPC::playAnim(NPCAnim anim, int speed, Direction dir) {
	if (dir == NONE)
		dir = this->dir;
	// Si la animaci�n no existe, seguro que no se puede
	std::string name = getAnimName(anim, dir);
	if (name == "")
		return false;

	// Si la animaci�n no tiene datos, algo va mal
	NPCAnimData data = getAnimationData(anim, dir);
	if (data.numFrames < 0)
		return false;

	if (speed < 0)
		speed = data.animSpeed;

	((SpriteMap*) graphic)->playAnim(name, speed, false, false);

	return true;
}

std::string NPC::getAnimName(NPCAnim anim, Direction dir) {
	if (dir == NONE) dir = this->dir;
	// Se obtiene el nombre de la animaci�n a partir del enum
	std::map<std::pair<NPCAnim, Direction>, std::string>::iterator it;
	it = animList.find(make_pair(anim, dir));
	// Si el iterador alcanca el final de la lista, no est� la anim
	if (it == animList.end())
		return "";
	else
		return (*it).second;
}
