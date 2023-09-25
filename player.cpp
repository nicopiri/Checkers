#include "player.hpp"

struct Board 
{
	string s;
	Board* next;
	Board* prev;
};
typedef Board* Pboard;

struct Player::Impl
{
	Pboard head;
	Pboard tail;
	int player;
	void destroy(Pboard head);
};

void Player::Impl::destroy(Pboard head)
{
	if (head) {
		destroy(head->next);
		delete head;
	}
}

Player::Player(int player_nr)
{
	if (player_nr != 1 && player_nr != 2) throw player_exception{ (player_exception::err_type::index_out_of_bounds), "Giocatore non definito"};
	pimpl = new Impl;
	pimpl->player = player_nr;
	pimpl->head = nullptr;
	pimpl->tail = nullptr;
}

Player::~Player()
{
	pimpl->destroy(pimpl->head);
	delete pimpl;
}

Player::Player(const Player& source)
{
	pimpl = new Impl;
	pimpl->player = source.pimpl->player;
	if (source.pimpl->head != nullptr && source.pimpl->tail != nullptr) {
		Pboard pb = source.pimpl->head;
		Pboard pboard = new Board;
		pboard->s = pb->s;
		pboard->next = nullptr;
		pboard->prev = nullptr;
		pimpl->head = pboard;
		pimpl->tail = pboard;
		pb = pb->next;
		while (pb) {
			Pboard pboard = new Board;
			pboard->next = nullptr;
			pboard->s = pb->s;
			pboard->prev = pimpl->tail;
			pimpl->tail->next = pboard;
			pimpl->tail = pboard;
			pb = pb->next;
		}
	}

}

Player& Player::operator=(const Player& s)
{
	if (s.pimpl->head == nullptr) {
		pimpl->head = nullptr;
		pimpl->tail = nullptr;
		pimpl->player = s.pimpl->player;
	}
	else {
		pimpl->destroy(pimpl->head);
		pimpl->head = nullptr;
		pimpl->tail = nullptr;
		Pboard pb = s.pimpl->head;
		pimpl->player = s.pimpl->player;
		while (pb) {
			Pboard pboard = new Board;
			pboard->next = nullptr;
			pboard->s = pb->s;
			pboard->prev = pimpl->tail;
			if (pimpl->tail != nullptr)
				pimpl->tail->next = pboard;
			else
				pimpl->head = pboard;
			pimpl->tail = pboard;
			pb = pb->next;
		}
	}
	return *this;
}


Player::piece Player::operator()(int r, int c, int history_offset) const 
{
	Pboard pc = pimpl->head;
	int len = 0;
	while (pc) {
		len++;
		pc = pc->next;
	}
	if (pimpl->tail == nullptr || len <= history_offset || history_offset < 0 || r > 7 || c > 7 || r < 0 || c < 0) throw player_exception{ player_exception::index_out_of_bounds, "coordinate inesistenti" };
	else {
		Pboard pb = pimpl->tail;
		for (int i = 0; i < history_offset; i++) {
			pb = pb->prev;
		}
		string s = pb->s;
		
		int pos= 105;
		for (int i = 0; i < r; i++) pos = pos - 15;
		pos = pos + (c*2);
		if (s[pos] == 'x') return x;
		else if (s[pos] == 'X') return X;
		else if (s[pos] == 'O') return O;
		else if (s[pos] == 'o') return o;
		else if (s[pos] == ' ') return e;
	}
	return e;
}

void Player::load_board(const string& filename) 
{
	
	fstream f(filename, ios::in);
	if (!f.good() || f.fail()) throw player_exception{ player_exception::missing_file, "scacchiera inesistente" };
	 
    int x = 0;
	int o = 0;
	bool ex = false;
	string row;
	int r = 0;
	
	while (getline(f, row)){
		if( row.length()!=15) throw player_exception{ player_exception::invalid_board, "scacchiera non valida 1" };
		int j = 2;
		int k = 0;
		if (r % 2 == 0) {
			j = 0;
			k = 2;
		}
		   for(int i =0; i<4; i++){
			if (row[j] == 'x') x++;
			else if (row[j] == 'X') x++;
			else if (row[j] == 'O') o++;
			else if (row[j] == 'o') o++;
			else if (row[j] != ' ') ex = true;

			if (row[k] != ' ') ex = true;

			k = k + 4;
			j = j + 4;
		}
          r++;
	}
	f.close();
	if(x>12 || o>12 || ex == true || r!=8) throw player_exception{ player_exception::invalid_board, "scacchiera non valida 2" };


		Pboard pboard = new Board;
		pboard->next = nullptr;
			string line;
			string s;
			string p;
			fstream f1(filename, ios::in);
			while (getline(f1, line)) s = s + line;
			f1.close();
			pboard->s = s;
		pboard->prev = pimpl->tail;
		if (pimpl->tail != nullptr)
			pimpl->tail->next = pboard;
		else
			pimpl->head = pboard;
		pimpl->tail = pboard;
}

void Player::store_board(const string& filename, int history_offset) const
{
	Pboard pc = pimpl->head;
	int len = 0;
	while (pc) {
		len++;
		pc = pc->next;
		}
	if (pimpl->tail == nullptr || len <= history_offset) {
		throw player_exception{ (player_exception::err_type::index_out_of_bounds), "scacchiera non disponibile" };
	}
	else {
		string s;
		pc = pimpl->tail;
		for (int i = 0; i < history_offset; i++) {
			pc = pc->prev;
		}
		s = pc->s;
		ofstream f;
		f.open(filename);
		
		for (int i = 0; i < 8; i++) {
			int k = 15 * i;
			string s1;
			for (int j=0; j < 15; j++) s1= s1 + s[j+k];
			f << s1 << endl;
		}

		f.close();
	}
	return;
}

void Player::init_board(const string& filename) const
{
	ofstream f;
	f.open(filename);
	
	f << "o   o   o   o  \n";
	f << "  o   o   o   o\n";
	f << "o   o   o   o  \n";
	f << "               \n";
	f << "               \n";
	f << "  x   x   x   x\n";
	f << "x   x   x   x  \n";
	f << "  x   x   x   x\n";

	f.close();
}

void Player::move()
{

int r=0, c=0, type=4, dir=0, n=0, updown=-1;
	if (pimpl->tail == nullptr) throw player_exception{ player_exception::index_out_of_bounds, "history vuota" };
	else {
		bool flag = false;
		
		if (pimpl->player == 1 && flag == false) {
			for (int i = 0; i <= 7; i++) {
				for (int j = 0; j <= 7; j++) {
					if ((*this)(i, j, 0) == 0 && flag == false) {

						if ((i + 2 >= 0 && j - 2 >= 0 && i + 2 <= 7 && j - 2 <= 7) && (*this)(i + 1, j - 1, 0) == 1 && (*this)(i + 2, j - 2, 0) == 4) {
							n = 2; r = i + 2; c = j - 2; type = (*this)(i, j, 0); dir = 1; flag = true;
						}
						else if ((i + 2 >= 0 && j + 2 >= 0 && i + 2 <= 7 && j + 2 <= 7) && (*this)(i + 1, j + 1, 0) == 1 && (*this)(i + 2, j + 2, 0) == 4) {
							n = 2; r = i + 2; c = j + 2; type = (*this)(i, j, 0); dir = -1; flag = true;
						}

					}

					else if ((*this)(i, j, 0) == 2 && flag == false) {
						if ((i - 2 >= 0 && j - 2 >= 0 && i - 2 <= 7 && j - 2 <= 7) && ((*this)(i - 1, j - 1, 0) == 1 || (*this)(i - 1, j - 1, 0) == 3) && (*this)(i - 2, j - 2, 0) == 4) {
							n = 2; r = i; c = j; type = (*this)(i, j, 0); dir = -1; flag = true; updown = 0;
						}
						else if ((i - 2 >= 0 && j + 2 >= 0 && i - 2 <= 7 && j + 2 <= 7) && ((*this)(i - 1, j + 1, 0) == 1 || (*this)(i - 1, j + 1, 0) == 3) && (*this)(i - 2, j + 2, 0) == 4) {
							n = 2; r = i; c = j; type = (*this)(i, j, 0); dir = 1; flag = true; updown = 0;
						}
						else	if ((i + 2 >= 0 && j - 2 >= 0 && i + 2 <= 7 && j - 2 <= 7) && ((*this)(i + 1, j - 1, 0) == 1 || (*this)(i + 1, j - 1, 0) == 3) && (*this)(i + 2, j - 2, 0) == 4) {
							n = 2; r = i + 2; c = j - 2; type = (*this)(i, j, 0); dir = 1; flag = true; updown = 1;
						}
						else if ((i + 2 >= 0 && j + 2 >= 0 && i + 2 <= 7 && j + 2 <= 7) && ((*this)(i + 1, j + 1, 0) == 1 || (*this)(i + 1, j + 1, 0) == 3) && (*this)(i + 2, j + 2, 0) == 4) {
							n = 2; r = i + 2; c = j + 2; type = (*this)(i, j, 0); dir = -1; flag = true; updown = 1;
						}

					}
				}
			}
			for (int i = 0; i <= 7; i++) {
				for (int j = 0; j <= 7; j++) {
					if ((*this)(i, j, 0) == 0 && flag == false) {
						if ((i + 1 >= 0 && j + 1 >= 0 && i + 1 <= 7 && j + 1 <= 7) && (*this)(i + 1, j + 1, 0) == 4) {

							n = 1; r = i + 1; c = j + 1; type = (*this)(i, j, 0); dir = -1; flag = true;
						}
						else if ((i + 1 >= 0 && j - 1 >= 0 && i + 1 <= 7 && j - 1 <= 7) && (*this)(i + 1, j - 1, 0) == 4) {
							n = 1; r = i + 1; c = j - 1; type = (*this)(i, j, 0); dir = 1; flag = true;
						}
						else n = 0;

					}

					else if ((*this)(i, j, 0) == 2 && flag == false) {
						if ((i - 1 >= 0 && j + 1 >= 0 && i - 1 <= 7 && j + 1 <= 7) && (*this)(i - 1, j + 1, 0) == 4) {
							n = 1; r = i; c = j; type = (*this)(i, j, 0); dir = 1; flag = true; updown = 0;
						}
						else if ((i - 1 >= 0 && j - 1 >= 0 && i - 1 <= 7 && j - 1 <= 7) && (*this)(i - 1, j - 1, 0) == 4) {
							n = 1; r = i; c = j; type = (*this)(i, j, 0); dir = -1; flag = true; updown = 0;
						}
						else if ((i + 1 >= 0 && j + 1 >= 0 && i + 1 <= 7 && j + 1 <= 7) && (*this)(i + 1, j + 1, 0) == 4) {
							n = 1; r = i + 1; c = j + 1; type = (*this)(i, j, 0); dir = -1; flag = true; updown = 1;
						}
						else if ((i + 1 >= 0 && j - 1 >= 0 && i + 1 <= 7 && j - 1 <= 7) && (*this)(i + 1, j - 1, 0) == 4) {
							n = 1; r = i + 1; c = j - 1; type = (*this)(i, j, 0); dir = 1; flag = true; updown = 1;
						}

						else n = 0;

					}
				}
			}
		}
		if (pimpl->player == 2 && flag == false) {
			for (int i = 7; i >= 0; i--) {
				for (int j = 0; j <= 7; j++) {
					if ((*this)(i, j, 0) == 1 && flag == false) {

						if ((i - 2 >= 0 && j - 2 >= 0 && i - 2 <= 7 && j - 2 <= 7) && (*this)(i - 1, j - 1, 0) == 0 && (*this)(i - 2, j - 2, 0) == 4) {
							n = 2; r = i; c = j; type = (*this)(i, j, 0); dir = -1; flag = true;
						}
						else if ((i - 2 >= 0 && j + 2 >= 0 && i - 2 <= 7 && j + 2 <= 7) && (*this)(i - 1, j + 1, 0) == 0 && (*this)(i - 2, j + 2, 0) == 4) {
							n = 2; r = i; c = j; type = (*this)(i, j, 0); dir = 1; flag = true;
						}

					}
					else if ((*this)(i, j, 0) == 3 && flag == false) {
						if ((i - 2 >= 0 && j - 2 >= 0 && i - 2 <= 7 && j - 2 <= 7) && (*this)(i - 1, j - 1, 0) == 0 && (*this)(i - 2, j - 2, 0) == 4) {
							n = 2; r = i; c = j; type = (*this)(i, j, 0); dir = -1; flag = true; updown = 0;
						}
						else if ((i - 2 >= 0 && j + 2 >= 0 && i - 2 <= 7 && j + 2 <= 7) && (*this)(i - 1, j + 1, 0) == 1 && (*this)(i - 2, j + 2, 0) == 4) {
							n = 2; r = i; c = j; type = (*this)(i, j, 0); dir = 1; flag = true; updown = 0;
						}
						else if ((i + 2 >= 0 && j - 2 >= 0 && i + 2 <= 7 && j - 2 <= 7) && (*this)(i + 1, j - 1, 0) == 1 && (*this)(i + 2, j - 2, 0) == 4) {
							n = 2; r = i + 2; c = j - 2; type = (*this)(i, j, 0); dir = 1; flag = true; updown = 1;
						}
						else if ((i + 2 >= 0 && j + 2 >= 0 && i + 2 <= 7 && j + 2 <= 7) && (*this)(i + 1, j + 1, 0) == 1 && (*this)(i + 2, j + 2, 0) == 4) {
							n = 2; r = i + 2; c = j + 2; type = (*this)(i, j, 0); dir = -1; flag = true; updown = 1;
						}


					}
				}
			}
			for (int i = 7; i >= 0; i--) {
				for (int j = 0; j <= 7; j++) {
					if ((*this)(i, j, 0) == 1 && flag == false) {

						if ((i - 1 >= 0 && j + 1 >= 0 && i - 1 <= 7 && j + 1 <= 7) && (*this)(i - 1, j + 1, 0) == 4) {

							n = 1; r = i; c = j; type = (*this)(i, j, 0); dir = 1; flag = true;
						}
						else if ((i - 1 >= 0 && j - 1 >= 0 && i - 1 <= 7 && j - 1 <= 7) && (*this)(i - 1, j - 1, 0) == 4) {
							n = 1; r = i; c = j; type = (*this)(i, j, 0); dir = -1; flag = true;
						}
						else n = 0;
					}
					else if ((*this)(i, j, 0) == 3 && flag == false) {
						if ((i + 1 >= 0 && j + 1 >= 0 && i + 1 <= 7 && j + 1 <= 7) && (*this)(i + 1, j + 1, 0) == 4) {
							n = 1; r = i + 1; c = j + 1; type = (*this)(i, j, 0); dir = -1; flag = true; updown = 1;
						}
						else if ((i + 1 >= 0 && j - 1 >= 0 && i + 1 <= 7 && j - 1 <= 7) && (*this)(i + 1, j - 1, 0) == 4) {
							n = 1; r = i + 1; c = j - 1; type = (*this)(i, j, 0); dir = 1; flag = true; updown = 1;
						}
						else if ((i - 1 >= 0 && j + 1 >= 0 && i - 1 <= 7 && j + 1 <= 7) && (*this)(i - 1, j + 1, 0) == 4) {
							n = 1; r = i; c = j; type = (*this)(i, j, 0); dir = 1; flag = true; updown = 0;
						}
						else if ((i - 1 >= 0 && j - 1 >= 0 && i - 1 <= 7 && j - 1 <= 7) && (*this)(i - 1, j - 1, 0) == 4) {
							n = 1; r = i; c = j; type = (*this)(i, j, 0); dir = -1; flag = true; updown = 0;
						}

						else n = 0;

					}
				}
			}
		}

	
		string ns = pimpl->tail->s;
		if (n != 0) {

			
			int pos = 105;
			for (int i = 0; i < r; i++) pos = pos - 15;
			pos = pos + (c * 2);
			int pos2 = pos + dir*2 +15;
			int pos3 = pos + dir*4 +30;
			
			     if( updown == -1 ) {
					 if (n == 1) {
						 if (type == 0) {
							 ns[pos] = 'x';
							 if(pos<15) ns[pos] = 'X';
							 ns[pos2] = ' ';
						 }
						 else if (type == 1) {
							 ns[pos] = ' ';
							 ns[pos2] = 'o';
							 if (pos2 > 94) ns[pos2] = 'O';
						 }
					 }else if (n == 2) {
						 if (type == 0) {
							 ns[pos] = 'x';
							 if (pos < 15) ns[pos] = 'X';
							 ns[pos2] = ' ';
							 ns[pos3] = ' ';
							
						 }else if (type == 1) {
							 ns[pos] = ' ';
							 ns[pos2] = ' ';
							 ns[pos3] = 'o';
							 if (pos3 > 94) ns[pos3] = 'O';

						 }
					 }
				 }
				 else if (updown != -1) {
					 if (n == 1) {
						 if (updown == 0) {
							 if (type == 2) {
								 ns[pos] = ' ';
								 ns[pos2] = 'X';
							 }if (type == 3) {
								 ns[pos] = ' ';
								 ns[pos2] = 'O';
							 }
						 }if (updown == 1) {
							 if (type == 2) {
								 ns[pos] = 'X';
								 ns[pos2] = ' ';
							 }if (type == 3) {
								 ns[pos] = 'O';
								 ns[pos2] = ' ';
							 }
						 }
					 }
					 else if (n == 2) {
						 if (updown == 0) {
							 if (type == 2) {
								 ns[pos] = ' ';
								 ns[pos2] = ' ';
								 ns[pos3] = 'X';
							 }if (type == 3) {
								 ns[pos] = ' ';
								 ns[pos2] = ' ';
								 ns[pos3] = 'O';
							 }
						 }if (updown == 1) {
							 if (type == 2) {
								 ns[pos] = 'X';
								 ns[pos2] = ' ';
								 ns[pos3] = ' ';
							 }if (type == 3) {
								 ns[pos] = 'O';
								 ns[pos2] = ' ';
								 ns[pos3] = ' ';
							 }
						 }
				  }
				 }
		}
		Pboard pboard = new Board;
		pboard->next = nullptr;
		pboard->s = ns;
		pboard->prev = pimpl->tail;
		if (pimpl->tail != nullptr)
			pimpl->tail->next = pboard;
		else
			pimpl->head = pboard;
		pimpl->tail = pboard;
	 
	}
}

bool Player::valid_move() const
{
	Pboard pb = pimpl->head; int nc = 0;
	while (pb) { nc++; pb = pb->next; }
	if(nc<2)throw player_exception{ (player_exception::err_type::index_out_of_bounds), "history minore di 2" };
	struct Cella {
		int r;
		int c;
		int type;
	};
	if (pimpl->tail->s == pimpl->tail->prev->s) return false;

	Cella mov[3];
	int n = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if ((*this)(i, j, 1) != (*this)(i, j, 0)) {
				if (n <= 2)mov[n] = { i, j, (*this)(i, j, 1) };
				n++;
			}
		}
	}
	if (n == 0) return true; 
	if (n !=2 && n!=3) return false;
	if (n == 2) mov[2] = mov[1]; 
	piece p0 = (*this)(mov[0].r, mov[0].c, 0);
	piece p1 = (*this)(mov[0].r, mov[0].c, 1);

	int diffC = mov[0].c - mov[2].c; 
	
	int r0, r1;
	if (p0 != 4) {
		r0 = mov[2].r;
		r1 = mov[0].r;
	}
	else {
		r0 = mov[0].r;
		r1 = mov[2].r;
	}
	int diffR = r0 - r1;

	if (diffC != 1 && diffC != -1 && n==2) return false;
	else if (diffC != 2 && diffC != -2 && n == 3) return false;

	if (p1 == 2 || p1 == 3 || p0 == 2 || p0 == 3) { 
		if (diffR != 1 && diffR != -1 && n == 2) return false;
		else if (diffR != 2 && diffR != -2 && n == 3) return false;
	}
	else if (p1 == 0 || p0 == 0) { 
		
		if (diffR != -1 && n == 2) return false;
		else if (diffR != -2 && n == 3) return false;
	}
	else if (p1 == 1 || p0 == 1) { 
		if (diffR != 1 && n == 2) return false;
		else if (diffR != 2 && n == 3) return false;
	}

	if (n == 3) {
		piece e0 = (*this)(mov[1].r, mov[1].c, 0);
		piece e1 = (*this)(mov[1].r, mov[1].c, 1);
		if (e0 != 4) return false;
		if (p1 == e1 || p0 == e1) return false;
		if (mov[0].c != mov[1].c + 1 && mov[0].c != mov[1].c - 1) return false;
		if (mov[0].r != mov[1].r + 1 && mov[0].r != mov[1].r - 1) return false;

	}
	

	return true;
}

void Player::pop()
{
	if (pimpl->tail == nullptr) throw player_exception{ (player_exception::err_type::index_out_of_bounds), "history vuota" };

	else {
		Pboard pb = pimpl->tail;
		if (pimpl->tail->prev) {
			pimpl->tail = pimpl->tail->prev;
			pimpl->tail->next = nullptr;

		}
		else {
			pimpl->head = nullptr;
			pimpl->tail = nullptr;
		}
		delete pb;
	}
}

bool Player::wins(int player_nr) const
{
	if (pimpl->tail == nullptr || player_nr<1 || player_nr>2) throw player_exception{ (player_exception::err_type::index_out_of_bounds), "player diverso da 1 o 2" };
	else {
		int x = 0;
		int o = 0;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if ((*this)(i, j, 0) == 0 || (*this)(i, j, 0) == 2) x++;
				else if ((*this)(i, j, 0) == 1 || (*this)(i, j, 0) == 3)  o++;
			}
		}
		if (player_nr == 1 && o == 0 && x > 0) return true;
		if (player_nr == 2 && x == 0 && o > 0) return true;
		return false;
	}
}

bool Player::wins() const
{
	if (pimpl->tail == nullptr) throw player_exception{ (player_exception::err_type::index_out_of_bounds), "history vuota" };
	else {
		int x = 0;
		int o = 0;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if ((*this)(i, j, 0) == 0 || (*this)(i, j, 0) == 2) x++;
				else if ((*this)(i, j, 0) == 1 || (*this)(i, j, 0) == 3)  o++;
			}
		}
		if (pimpl->player == 1 && o == 0 && x > 0) return true;
		if (pimpl->player == 2 && x == 0 && o > 0) return true;
		return false;
	}
}

bool Player::loses(int player_nr) const
{
	if (pimpl->tail == nullptr || player_nr < 1 || player_nr>2) throw player_exception{ (player_exception::err_type::index_out_of_bounds), "player diverso da 1 o 2" };
	else {
	
		int x = 0;
		int o = 0;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if ((*this)(i, j, 0) == 0 || (*this)(i, j, 0) == 2) x++;
				else if ((*this)(i, j, 0) == 1 || (*this)(i, j, 0) == 3)  o++;
			}
		}
		if (player_nr == 1 && x == 0 && o > 0) return true;
		if (player_nr == 2 && o == 0 && x > 0) return true;
		return false;
	}
}

bool Player::loses() const 
{
	if (pimpl->tail == nullptr) throw player_exception{ (player_exception::err_type::index_out_of_bounds), "history vuota" };
	else {
		int x = 0;
		int o = 0;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if ((*this)(i, j, 0) == 0 || (*this)(i, j, 0) == 2) x++;
				else if ((*this)(i, j, 0) == 1 || (*this)(i, j, 0) == 3)  o++;
			}
		}
		if (pimpl->player == 1 && x == 0 && o > 0) return true;
		if (pimpl->player == 2 && o == 0 && x > 0) return true;
		return false;
	}
}

int Player::recurrence() const
{
	if(pimpl->tail==nullptr)throw player_exception{ (player_exception::err_type::index_out_of_bounds), "history vuota" };
	else {
		Pboard pb = pimpl->head;
		int n = 0;
		while (pb) {
	
			if (pb->s == pimpl->tail->s) n++;
			pb = pb->next;
		}
		return n;
	}
}
