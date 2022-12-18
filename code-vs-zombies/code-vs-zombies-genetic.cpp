#include <algorithm>
#include <climits>
#include <cmath>
#include <iostream>
#include <iterator>
#include <random>
#include <string>
#include <vector>

using namespace std;

class Vector2 {
 public:
  int x;
  int y;

  Vector2(int x = 0, int y = 0) {
    this->x = x;
    this->y = y;
  }

  int distance(Vector2 point2) {
    Vector2 point1 = *this;
    return abs(point1.x - point2.x) + abs(point2.y - point2.y);
  }

  Vector2 closest_point(vector<Vector2> points, int starting_index = 0) {
    Vector2 point = *this;
    int min_distance = INT_MAX;
    Vector2 closest_point;
    for (int i = starting_index; i < points.size(); i++) {
      int distance = point.distance(points[i]);
      if (distance < min_distance) {
        min_distance = distance;
        closest_point = points[i];
      }
    }
    return closest_point;
  }

  Vector2 move_toward(Vector2 target, int units_to_move) {
    Vector2 current = *this;
    // Okay let's try copy unity instead
    float to_vector_x = target.x - current.x;
    float to_vector_y = target.y - current.y;

    float sq_dist = to_vector_x * to_vector_x + to_vector_y * to_vector_y;
    // This might be wrong
    if (sq_dist == 0 || (units_to_move >= 0 && sq_dist <= units_to_move * units_to_move)) {
      return target;
    }

    float dist = sqrt(sq_dist);
    // This might also be wrong
    return *new Vector2(current.x + to_vector_x / dist * units_to_move, current.y + to_vector_y / dist * units_to_move);
  }

  bool equals(Vector2 point2) {
    Vector2 point1 = *this;
    if (point1.x == point2.x && point1.y == point2.y) {
      return true;
    }
    return false;
  }

  friend ostream &operator<<(ostream &os, const Vector2 &v) {
    os << "X: " << v.x << " Y: " << v.y;
    return os;
  }
};

class Game {
 public:
  const int zombie_speed = 400;
  const int player_speed = 1000;
  const int player_range = 2000;
  vector<Vector2> zombie_positions;
  vector<Vector2> human_positions;
  Vector2 player_postion;
  int score = 0;

  Vector2 nearest_human(Vector2 zombie_position) {
    return zombie_position.closest_point(human_positions);
  }

  int distance_to_player(Vector2 zombie_position) {
    return player_postion.distance(zombie_position);
  }

  void move_zombies() {
    // Each zombie moves towards the closest human including the player
    for (int i = 0; i < zombie_positions.size(); i++) {
      Vector2 zombie = zombie_positions[i];
      Vector2 human = nearest_human(zombie);
      // I have a feeling this is a good place for a pointer but nobody understand pointers
      zombie_positions[i] = zombie.move_toward(human, zombie_speed);
    }
  }

  // Stole the fibonacci code from geeks4geeks sorry world
  int calculate_score(int zombies_killed) {
    vector<int> f(zombies_killed + 2);  // this seems fine...what up why do people use arrays in c++?
    int i;

    f[0] = 0;
    f[1] = 1;
    for (i = 2; i <= zombies_killed; i++) {
      f[i] = f[i - 1] + f[i - 2];
    }
    return f[zombies_killed];
  }

  void shoot_zombies() {
    // Ugly but whatever
    int count = 0;
    int zombies_killed = 0;
    for (auto i = zombie_positions.begin(); i != zombie_positions.end(); i++) {
      Vector2 zombie = zombie_positions[count];
      if (distance_to_player(zombie) < 2000) {
        // Remove zombie from zombie_positions
        zombie_positions.erase(i);  // This only erases one right? Not the position -> end
        // Add score
        zombies_killed++;
      } else {
        count++;
      }
    }
    score += calculate_score(zombies_killed);
  }

  void eat_humans() {
    // If zombie shares square with human remove human from human_positions
    int count = 0;
    for (auto i = human_positions.begin(); i != human_positions.end(); i++) {
      Vector2 human = human_positions[count];
      for (Vector2 zombie : zombie_positions) {
        if (zombie.equals(human)) {
          human_positions.erase(i);
          count--;  // I hate this
        }
      }
      count++;  // This might not be placed correctly
    }
  }

  void move_player(char direction) {
  }

  void simulate_game_round(char direction) {
    move_zombies();
    // for (Vector2 zombie : zombie_positions) {
    //   cerr << zombie.x << " " << zombie.y << endl;
    // }
    // Might come back to this when implementing the genetic algorithm
    move_player(direction);
    shoot_zombies();  // do scoring here
    eat_humans();
  }

  void simulate_game(string chrom) {
    for (int i = 0; i < chrom.size(); i++) {
      simulate_game_round(chrom[i]);
    }
  }

  bool check_victory(string moves) {
    return true;  // TODO
  }
};
// Do I want a new class for my genetic algorithm?
// What I need to do is simulate the game so I can measure the chromosome's success

class Genetics {
 public:
  // Game game;

  // Genetics(Game gameInstance) {
  // }

  template <typename Iter, typename RandomGenerator>
  Iter select_randomly(Iter start, Iter end, RandomGenerator &g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
  }

  template <typename Iter>
  Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
  }
  // X = UL, Y = DL, Z = UR, Q = DR
  vector<string> directions{"U", "D", "L", "R", "X", "Y", "Z", "Q"};

  string create_chromosome(int chrom_size) {
    string chrom;
    for (int i = 0; i < chrom_size; i++) {
      chrom += *select_randomly(directions.begin(), directions.end());
    }
    return chrom;
  }

  int get_score(string chrom, Game game) {
    // TODO: Call game simulation stuff here
  }

  vector<string> selection(vector<string> population, Game game) {
    const float selection_percentage = 0.3;
    const float random_percentage = 0.2;

    vector<pair<int, string>> population_scores;
    for (string chrom : population) {
      int score = get_score(chrom, game);
      // I can just use a pair
      pair<int, string> temp;
      temp.first = score;
      temp.second = chrom;
      population_scores.push_back(temp);
    }

    // TODO: Sort by pair.second (I'm tired)
  }

  string crossover(string parent1, string parent2) {
    // Combine half of parent1 and half of parent2
  }

  string mutation(string child) {
    // Select 1 random direction and modify it with a new one
  }

  vector<string> create_population(int pop_size, int chrom_size) {
    vector<string> population;
    for (int i = 0; i < pop_size; i++) {
      string chrom = create_chromosome(chrom_size);
      population.push_back(chrom);
    }
  }

  vector<string> generation(vector<string> population, Game game) {
    vector<string> select = selection(population, game);
    vector<string> children;
    while (children.size() < population.size() - select.size()) {
      string parent1 = *select_randomly(population.begin(), population.end());
      string parent2 = *select_randomly(population.begin(), population.end());
      string child = crossover(parent1, parent2);

      child = mutation(child);  // Probably need to make this less common
      children.push_back(child);
    }
    vector<string> result;
    result.reserve(select.size() + children.size());
    result.insert(result.end(), children.begin(), children.end());
    result.insert(result.end(), select.begin(), select.end());
    return result;
  }

  string algorithm(Game game) {
    int chrom_size = 30;  // Max turn length?
    int population_size = 30;
    int iterations = 20000;
    vector<string> population = create_population(population_size, chrom_size);
    vector<string> answers;
    int count;

    while (count < iterations) {
      population = generation(population, game);
      for (string chrom : population) {
        // Need to check for victory in simulation
        if (game.check_victory(chrom)) {
          answers.push_back(chrom);
        }
      }
      count++;
    }
  }
};

int main() {
  // game loop
  Game game;
  while (1) {
    int x;
    int y;
    cin >> x >> y;
    cin.ignore();
    int human_count;
    cin >> human_count;
    cin.ignore();
    vector<Vector2> human_positions;
    for (int i = 0; i < human_count; i++) {
      int human_id;
      int human_x;
      int human_y;
      cin >> human_id >> human_x >> human_y;
      cin.ignore();
      Vector2 human_position = *new Vector2(human_x, human_y);
      human_positions.push_back(human_position);
    }
    game.human_positions = human_positions;
    int zombie_count;
    cin >> zombie_count;
    cin.ignore();
    vector<Vector2> zombie_positions;
    for (int i = 0; i < zombie_count; i++) {
      int zombie_id;
      int zombie_x;
      int zombie_y;
      int zombie_xnext;
      int zombie_ynext;
      cin >> zombie_id >> zombie_x >> zombie_y >> zombie_xnext >> zombie_ynext;
      cin.ignore();
      Vector2 zombie_position = *new Vector2(zombie_x, zombie_y);
      zombie_positions.push_back(zombie_position);
    }
    game.zombie_positions = zombie_positions;

    Genetics genetics;
    // I might need to make a copy of game at some point, dunno, brain is explode
    string answer = genetics.algorithm(game);

    cout << 0 << " " << 0 << endl;  // For debugging
  }
}
// That looks like a nasty error
