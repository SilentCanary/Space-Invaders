#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <time.h>
#include <random>
#include <ctime>
#include <fstream>

using namespace std;
using namespace sf;

enum GameState
{
    NAME_INPUT,
    MENU,
    PLAY,
    GAME_OVER,
    LEADERBOARD
};

//Basic binary tree structure for maintaing and calculating position in leaderboard
struct TreeNode
{
    TreeNode* left;
    TreeNode* right;
    int score;
    string player_name;
    TreeNode(int s,string name):left(nullptr),right(nullptr),score(s),player_name(name){}
};

//functions of bst 
class BST
{
    TreeNode* parent;

    TreeNode* insert_node(TreeNode* node,int score,string name)
    {
        if(!node) return new TreeNode(score,name);
        if(score<node->score) node->left=insert_node(node->left,score,name);
        else if(score>node->score) node->right=insert_node(node->right,score,name);
        return node;
    }
    void inorder_traversal(TreeNode* root,vector<pair<int,string>>&scores)
    {
        if(root)
        {
            inorder_traversal(root->right,scores);
            scores.push_back({root->score,root->player_name});
            inorder_traversal(root->left,scores);
        }
    }
    void clear(TreeNode* root)
    {
        if(root)
        {
            clear(root->left);
            clear(root->right);
            delete root;
        }
    }
    public:
    BST():parent(nullptr){}
    void insert(int score,string name)
    {
        parent=this->insert_node(parent,score,name);
    }
    vector<pair<int,string>> get_scores()
    {
        vector<pair<int,string>>SCORES;
        this->inorder_traversal(parent,SCORES);
        return SCORES;
    }
    ~BST()
    {
        this->clear(this->parent);
    }
};


//class for handling saving and loading files
class SaveFile
{
    fstream savefile;
    public:
    void save_file(vector<pair<int,string>>&scores)
    {
        savefile.open("LEADERBOARD.txt",ios::out);
        if(savefile.is_open())
        {
            for(const auto& score:scores)
            {
                savefile<<score.second<<" "<<score.first<<endl;
                cout<<score.second<<" "<<score.first<<endl;
            }
            savefile.close();
        }
    }
    void loadFile(BST& leaderboard)
    {
        savefile.open("LEADERBOARD.txt",ios::in);
        if (savefile.is_open())
        {
            string name;
            int score;
            while(savefile>>name>>score)
            {
                leaderboard.insert(score,name);
            }
            savefile.close();
        }
        
    }

};


//the first menu to take input from user and play button
class name_input
{
    Sprite background;
    Texture background_texture;
    RectangleShape text_box;
    bool isActive;
    string name;
    Font font;
    Text text;
    Text title_text;
    CircleShape  go_button;
    ConvexShape arrow;
   void init_go_button()
   {
      go_button.setRadius(20.f);
      go_button.setFillColor(Color(25,25,25,200));
      go_button.setOutlineColor(Color::White);
      go_button.setOutlineThickness(2.f);
      go_button.setOrigin(20.f,20.f);
      go_button.setPosition(400.f,400.f);
      arrow.setPointCount(3);
      arrow.setFillColor(Color::White);
      arrow.setOutlineColor(Color::Black);
      arrow.setPoint(0,Vector2f(0, -25));
      arrow.setPoint(1,Vector2f(12.5, 0));
      arrow.setPoint(2,Vector2f(-12.5, 0));
      arrow.setRotation(90);
      arrow.setOrigin(0,-25); // Set origin to top-center of the triangle
      arrow.setPosition(go_button.getPosition().x+15.f,go_button.getPosition().y);
   }
    public:
    void init_text_box()
    {
        this->text_box.setSize(Vector2f(300.f, 50.f));
        this->text_box.setFillColor(Color(25, 25, 25, 200));
        this->text_box.setOutlineColor(Color::White);
        this->text_box.setOutlineThickness(1.f);
        this->text_box.setPosition(250.f, 300.f);

        this->text.setFont(font);
        this->text.setCharacterSize(20);
        this->text.setFillColor(Color::White);
        this->text.setPosition(260.f, 310.f);
        isActive=false;
    }
    name_input(RenderTarget& target)
    {
        this->background_texture.loadFromFile("C:/Users/sairam/Documents/data structures/graph/Texture/last.jpg");
        this->background.setTexture(this->background_texture);
        this->background.setScale(
        float(target.getSize().x)/this->background_texture.getSize().x,
        float(target.getSize().y)/this->background_texture.getSize().y);
        this->font.loadFromFile("C:/Users/sairam/Documents/data structures/graph/Texture/ARCADE.ttf");
        this->title_text.setFont(this->font);
        this->title_text.setCharacterSize(50);
        this->title_text.setFillColor(Color::White);
        this->title_text.setPosition(200.f,200.f);
        this->title_text.setString("SPACE INVADERS");
        this->init_text_box();
        this->init_go_button();
    }
    void handle_event(Event ev, RenderWindow& window)
    {
        if (ev.type == Event::MouseButtonPressed)
        {
            if (text_box.getGlobalBounds().contains(static_cast<Vector2f>(Mouse::getPosition(window))))
            {
                isActive = true;
                cout << "TEXT BOX ACTIVE" << endl;
            }
            else
            {
                isActive = false;
                cout << "TEXT BOX INACTIVE" << endl;
            }
        }
        else if (ev.type == Event::TextEntered && isActive)
        {
            if (ev.text.unicode == '\b' && !name.empty())
            {
                name.pop_back(); // Handle backspace to delete characters
            }
            else if (ev.text.unicode < 128 && ev.text.unicode != '\b')
            {
                name += static_cast<char>(ev.text.unicode); // Append character to name
            }
            text.setString(name); // Update text display
        }
    }

    bool is_Mouse_hover(RenderWindow& window)
    {
       Vector2i mouse_pos=Mouse::getPosition(window);
       FloatRect getbounds=go_button.getGlobalBounds();
       return getbounds.contains(static_cast<Vector2f>(mouse_pos));
    }

  void update_color(RenderWindow& window)
  {
    if(is_Mouse_hover(window))
    {
        go_button.setFillColor(Color(50,50,50,200));
    }
    else
    {
        go_button.setFillColor(Color(25,25,25,200));
    }
  }
    const string get_name()
    {
        return this->name;
    }

    void render(RenderWindow& window)
    {
        window.draw(this->background);
        window.draw(this->title_text);
        window.draw(this->text_box);
        window.draw(this->text);
        update_color(window);
        window.draw(this->go_button);
        window.draw(this->arrow);
    }
};


//welcome page and start button
class Menu
{
  Sprite background;
  Texture background_texture;
  Text title_text;
  Text start_text;
  Font font;
  RectangleShape button;
  
  public:

  Menu(RenderTarget& target,const string player_name)
  {
    this->background_texture.loadFromFile("C:/Users/sairam/Documents/data structures/graph/Texture/last.jpg");
    this->background.setTexture(this->background_texture);
    this->background.setScale(
        float(target.getSize().x)/this->background_texture.getSize().x,
        float(target.getSize().y)/this->background_texture.getSize().y);
    this->font.loadFromFile("C:/Users/sairam/Documents/data structures/graph/Texture/ARCADE.ttf");
    this->title_text.setFont(this->font);
    this->title_text.setCharacterSize(50);
    this->title_text.setFillColor(Color::White);
    this->title_text.setPosition(200.f,200.f);
    this->title_text.setString("WELCOME "+player_name);

    this->start_text.setFont(this->font);
    this->start_text.setFillColor(Color::White);
    this->start_text.setCharacterSize(40);
    this->start_text.setPosition(300.f,400.f);
    this->start_text.setString("START");
    FloatRect getbounds=start_text.getGlobalBounds();

    this->button.setSize(Vector2f(getbounds.width+20.f,getbounds.height+10.f));
    this->button.setFillColor(Color(25,25,25,200));
    this->button.setOutlineColor(Color::White);
    this->button.setOutlineThickness(1.f);
    this->button.setPosition(this->start_text.getPosition().x - 10.f,
            this->start_text.getPosition().y + 8.f
        );

  }

  bool is_Mouse_hover(RenderWindow& window)
  {
    Vector2i mouse_pos=Mouse::getPosition(window);
    FloatRect getbounds=button.getGlobalBounds();
    return getbounds.contains(static_cast<Vector2f>(mouse_pos));
  }

  void update_color(RenderWindow& window)
  {
    if(is_Mouse_hover(window))
    {
        button.setFillColor(Color(50,50,50,200));
    }
    else
    {
        button.setFillColor(Color(25,25,25,200));
    }
  }
  void render(RenderWindow& window)
  {
    window.draw(this->background);
    window.draw(this->title_text);
    update_color(window);
    window.draw(this->button);
    window.draw(this->start_text);
    
  }
};

class Bullet
{
    Sprite sprite;
    Texture texture;
    Vector2f direction;
    float movement_speed;
    void init_texture()
    {
        if(!this->texture.loadFromFile("C:/Users/sairam/Documents/data structures/graph/Texture/bullet3.png"))
        {
            cout<<"COULDN'T OPEN THE IMAGE"<<endl;
        };
    }
    public:
    Bullet(float pos_x,float dx,float pos_y,float dy,float movement)
    {
        this->init_texture();
        this->sprite.setTexture(this->texture);
        this->sprite.setScale(0.6f,0.6f);
        this->sprite.setPosition(pos_x,pos_y);
        this->direction.x=dx;
        this->direction.y=dy;
        this->movement_speed=movement;
        cout << "Bullet created at position (" << pos_x << ", " << pos_y << ")" << endl;
    }

    FloatRect getbounds()const
    {
        return this->sprite.getGlobalBounds();
    }

    void update()
    {
        this->sprite.move(this->movement_speed*direction);
    }

    void render(RenderTarget &target)
    {
        target.draw(this->sprite);
    }

};

class Enemy
{
    Sprite sprite;
    Texture texture;
    int points;
    float speed;
    void init_texture()
    {
        if(!this->texture.loadFromFile("C:/Users/sairam/Documents/data structures/graph/Texture/monster1r.png"))
        {
            cout<<"COULDN'T LOAD IMAGE!!"<<endl;
        }
    }
public:
    Enemy(float pos_x,float pos_y)
    {
        this->init_texture();
        this->sprite.setTexture(this->texture);
        this->sprite.setPosition(pos_x,pos_y);
        this->speed=2.f;
        this->points=10;
    }

    void update()
    {
        this->sprite.move(0.f,this->speed);
    }

    FloatRect getbounds() const
    {
        return this->sprite.getGlobalBounds(); 
    }
    void render(RenderTarget &target)
    {
        target.draw(this->sprite);
    }

    const int get_points()
    {
        return this->points;  //got too lazy i did this cuz initially i planned on having different types of monster in a vector and then randomly generate where each would have differnet value 
    }
    ~Enemy()
    {

    }
};

class Player
{
    Sprite sprite;
    Texture texture;
    int hp;
    int hp_max;
    bool isHit;
    float HitTimer;
    float movement_speed;

    void init_texture()
    {
        if(!this->texture.loadFromFile("C:/Users/sairam/Documents/data structures/graph/Texture/shoot.png"))
        {
            cout<<"COULDN'T OPEN THE IMAGE"<<endl;
        }
    }

    void init_sprite()
    {
        this->sprite.setTexture(this->texture);
        this->sprite.scale(0.4f,0.4f);
    }

    
    public:
    Player()
    {
        this->init_texture();
        this->init_sprite();
        this->movement_speed=10.f;
        this->hp_max=50;
        this->isHit=false;
        this->HitTimer=0.f;
        this->hp=this->hp_max;
    }

    const int get_hp()
    {
        return this->hp;
    }

    const int get_hp_max()
    {
        return this->hp_max;
    }

    void set_hp(int hp)
    {
        this->hp=hp;
    }

    void lose_hp(int val)
    {
        this->hp-=val;
        if(this->hp<0) this->hp=0;
        this->isHit=true;
        this->HitTimer=1.f;
    }

    void set_position(Vector2f pos)
    {
        this->sprite.setPosition(pos);
    }
    const FloatRect getbounds()const
    {
        return this->sprite.getGlobalBounds();
    }
    void move(float dx,float dy)
    {
        this->sprite.move(this->movement_speed*dx,this->movement_speed*dy);
    }
    void render(RenderTarget &target)
    {
        target.draw(sprite);
    }

    const Vector2f  get_position()
    {
        return this->sprite.getPosition();
    }
};

class Game
{
    RenderWindow *window;
    Event ev;
    Player *player;
    vector<Enemy*>enemies;
    Menu *menu;
    vector<Bullet*>bullets;
    float spawn_timer;
    float spawn_timer_max;
    Sprite background;
    Texture background_texture;
    Font font;

    //--------------------------------------------------------------
    GameState state;
    name_input *input;
    RectangleShape hp_bar;
    RectangleShape hp_bar_back;
    string player_name;
    int points;
    Text point_text;

    Sprite gameover_background;
    Text GameOverText;
    Texture gameover_texture;

    BST leaderboard;
    RectangleShape button;
    Text leaderboard_button_text;
    SaveFile file;
//--------------leaderboard button on game over page------------
    void init_button()
    {
        this->leaderboard_button_text.setFont(this->font);
        this->leaderboard_button_text.setFillColor(Color::White);
        this->leaderboard_button_text.setCharacterSize(40);
        this->leaderboard_button_text.setPosition(300.f,400.f);
        this->leaderboard_button_text.setString("LEADERBOARD");
        FloatRect getbounds=leaderboard_button_text.getGlobalBounds();

        this->button.setSize(Vector2f(getbounds.width+20.f,getbounds.height+10.f));
        this->button.setFillColor(Color(25,25,25,200));
        this->button.setOutlineColor(Color::White);
        this->button.setOutlineThickness(1.f);
        this->button.setPosition(this->leaderboard_button_text.getPosition().x - 10.f,
         this->leaderboard_button_text.getPosition().y + 8.f
        );

    }
    bool is_Mouse_hover()
    {
      Vector2i mouse_pos=Mouse::getPosition(*this->window);
      FloatRect getbounds=button.getGlobalBounds();
      return getbounds.contains(static_cast<Vector2f>(mouse_pos));
    }

    void update_color()
  {
    if(is_Mouse_hover())
    {
        button.setFillColor(Color(50,50,50,200));
    }
    else
    {
        button.setFillColor(Color(25,25,25,200));
    }
  }
//--------------------------------------------------------------------  

    void render_leaderboard()
    {
        this->window->draw(this->gameover_background);
        Text title("Leaderboard", this->font, 50);
        title.setFillColor(Color::White);
        title.setPosition(200.f,200.f);
        this->window->draw(title);
        vector<pair<int, string>> scores = this->leaderboard.get_scores();
        float y_position = 350.f;

        for (const auto& score : scores)
        {
            Text scoreText;
            scoreText.setFont(this->font);
            scoreText.setCharacterSize(30);
            scoreText.setFillColor(Color::White);
            scoreText.setString(score.second + " : " + to_string(score.first));
            scoreText.setPosition(100.f, y_position);
            this->window->draw(scoreText);
            y_position += 40.f;
        }
    }

    void init_background()
    {
        this->background_texture.loadFromFile("C:/Users/sairam/Documents/data structures/graph/Texture/space3.jpeg");
        this->background.setTexture(this->background_texture);
        // Scale background to fit the window size
        this->background.setScale( float(this->window->getSize().x) / this->background_texture.getSize().x,
        float(this->window->getSize().y) / this->background_texture.getSize().y);
        this->gameover_texture.loadFromFile("C:/Users/sairam/Documents/data structures/graph/Texture/last.jpg");
        this->gameover_background.setTexture(this->gameover_texture);
        this->gameover_background.setScale(
        float(this->window->getSize().x) / this->gameover_texture.getSize().x,
        float(this->window->getSize().y) / this->gameover_texture.getSize().y);
    }

    void init_gui()
    {
        this->font.loadFromFile("C:/Users/sairam/Documents/data structures/graph/Texture/ARCADE.ttf");
        this->point_text.setFont(this->font);
        this->point_text.setCharacterSize(28);
        this->point_text.setFillColor(Color::White);
        this->point_text.setPosition(630.f,25.f);
        this->points=0;
        this->hp_bar.setSize(Vector2f(200.f,25.f));
        this->hp_bar.setFillColor(Color(250,0,0));
        this->hp_bar.setPosition(Vector2f(20.f,20.f));
        this->hp_bar_back=this->hp_bar;
        this->hp_bar_back.setFillColor(Color(25,25,25,200));
        this->GameOverText.setFont(this->font);
        this->GameOverText.setCharacterSize(60);
        this->GameOverText.setFillColor(Color::White);
        this->GameOverText.setPosition(this->window->getSize().x/2.f-this->GameOverText.getGlobalBounds().width-150.f,
        this->window->getSize().y/2.f-this->GameOverText.getGlobalBounds().height/2);
        this->GameOverText.setString("GAME OVER");
    }
    
    public:
    Game()
    {   
        this->window=new RenderWindow(VideoMode(800,600),"game",Style::Fullscreen|Style::Titlebar);
        this->window->setFramerateLimit(144);
        //this->menu=new Menu(*this->window,);
        this->input=new name_input(*this->window);
        this->state=NAME_INPUT;
        this->init_background();
        this->player=new Player();
        this->player->set_position({400.f - this->player->getbounds().width / 2.f, 600.f-this->player->getbounds().height});
        this->spawn_timer_max=50.f;
        this->spawn_timer=this->spawn_timer_max;
        this->init_gui();
        this->init_button();
        this->file.loadFile(this->leaderboard);
    }
    
    
    void update_gui()
    {
        stringstream ss;
        ss<<"POINTS : "<<points;
        this->point_text.setString(ss.str());
        float hp_percent=static_cast<float>(this->player->get_hp())/this->player->get_hp_max();
        this->hp_bar.setSize(Vector2f(200.f*hp_percent,this->hp_bar.getSize().y));
    }


    void update_collision()
    {
        if(this->player->getbounds().left<0.f)
        {
            this->player->set_position({0.f,this->player->getbounds().top});

        }

        else if(this->player->getbounds().left+this->player->getbounds().width >= this->window->getSize().x)
        {
            this->player->set_position({this->window->getSize().x-this->player->getbounds().width,this->player->getbounds().top});
        }
        
        if(this->player->getbounds().top < 0.f)
        {
            this->player->set_position({this->player->getbounds().left,0.f});
        }
        
        if(this->player->getbounds().top+this->player->getbounds().height>this->window->getSize().y)
        {
            this->player->set_position({this->player->getbounds().left,this->window->getSize().y-this->player->getbounds().height});
        }
        
    }

    void poll_events()
    {
        
    while (this->window->pollEvent(this->ev))
    {
        switch (this->state)
        {
        case NAME_INPUT:
            this->input->handle_event(this->ev, *this->window);
            if (this->ev.type == Event::MouseButtonPressed && this->ev.mouseButton.button == Mouse::Left)
            {
                if (this->input->is_Mouse_hover(*this->window))
                {
                    this->player_name = this->input->get_name();
                    delete this->input;
                    this->menu = new Menu(*this->window, this->player_name);
                    this->state = MENU;
                }
            }
            break;
        case MENU:
            if (this->ev.type == Event::MouseButtonPressed && this->ev.mouseButton.button == Mouse::Left)
            {
                if (this->menu->is_Mouse_hover(*this->window))
                {
                    this->state = PLAY;
                }
            }
            break;
        case PLAY:
            if (this->ev.type == Event::Closed)
            {
                this->window->close();
            }
            else if (this->ev.type == Event::KeyPressed)
            {
                if(this->ev.key.code == Keyboard::Escape)
                {
                    this->window->close();
                    break;
                }
                else if(ev.key.code==Keyboard::Left)
               {
                this->player->move(-1.f,0.f);
               }
               else if(ev.key.code==Keyboard::Right)
               {
                this->player->move(1.f,0.f);
               }
               else if(ev.key.code==Keyboard::Up)
               {
                this->player->move(0.f,-1.f);
               }
               else if(ev.key.code==Keyboard::Down)
               {
                this->player->move(0.f,1.f);
               }
               else if(ev.key.code==Keyboard::Space)
               {
                this->bullets.push_back(new Bullet(this->player->get_position().x+this->player->getbounds().width/2.f,
                0.f,this->player->get_position().y,-1.f,5.f));
               }
            }
            break;
        case GAME_OVER:
             if (this->ev.type == Event::MouseButtonPressed && this->ev.mouseButton.button == Mouse::Left)
            {
                if (this->is_Mouse_hover())
                {
                    this->state = LEADERBOARD;
                }
            }
             else if(this->ev.type==Event::KeyPressed)
             {
                if(ev.key.code==Keyboard::Escape) this->window->close();
             }
             break;
        case LEADERBOARD:
             if(this->ev.type==Event::KeyPressed)
             {
                if(ev.key.code==Keyboard::Escape) this->window->close();
             } 
             break;
        default:
            break;
        }
    }
    } 

    void update_bullets()
    {
        int count=0;
        for(int i=0;i<bullets.size();i++)
        {
            bullets[i]->update();
            if(bullets[i]->getbounds().top+bullets[i]->getbounds().height<0.f)
            {
                delete this->bullets[i];
                this->bullets.erase(this->bullets.begin()+i);
            }
        }
    }

    void update_enemies()
    {
        this->spawn_timer+=0.5f;
        if(this->spawn_timer>=this->spawn_timer_max)
        {
            this->enemies.push_back(new Enemy(rand()%800,-100.f));
            this->spawn_timer=0.f;
        }
        for(int i=0;i<enemies.size();i++)
        {
            bool enemy_active=true;
            enemies[i]->update();

            if(enemy_active && enemies[i]->getbounds().top>this->window->getSize().y)
            {
                enemies.erase(enemies.begin()+i);
                cout<<this->enemies.size()<<endl;
            }

            else if(enemies[i]->getbounds().intersects(this->player->getbounds()))
            {
                enemies.erase(enemies.begin()+i);
                cout<<this->enemies.size()<<endl;
                this->player->lose_hp(5);
            }

            for(int k=0;k<bullets.size() && enemy_active;k++)
            {
                if( bullets[k]->getbounds().intersects(enemies[i]->getbounds()))
                {
                    this->points+=enemies[i]->get_points();
                    delete bullets[k];
                    bullets.erase(bullets.begin()+k);
                    delete enemies[i];
                    enemies.erase(enemies.begin()+i);
                    enemy_active=false;
                }
            }
        }
    }
    
    void update()
    {
        if(this->state==PLAY)
        {
           this->update_collision();
           this->update_bullets();
           this->update_enemies();
           this->update_gui();
           if(this->player->get_hp()<=0) 
           {
             this->state=GAME_OVER;
             leaderboard.insert(this->points,this->player_name);
             cout<<player_name<<endl;
             vector<pair<int,string>>scores=leaderboard.get_scores();
             file.save_file(scores);
           }
        }
            
    }

    void render_gui()
    {
        this->window->draw(this->point_text);
        this->window->draw(this->hp_bar_back);
        this->window->draw(this->hp_bar);
    }
    
    void render_background()
    {
        this->window->draw(this->background);
    }
    

    void render_game_over_screen()
    {
        this->window->draw(this->gameover_background);
        this->window->draw(this->GameOverText);
        this->window->draw(this->button);
        update_color();
        this->window->draw(this->leaderboard_button_text);
    }

    void render()
    {
        this->window->clear();
        if(this->state==NAME_INPUT)
        {
            this->input->render(*this->window);
        }
        if(this->state==MENU)
        {
            this->menu->render(*this->window);
        }
        else if(this->state==PLAY)
        {
           this->render_background();
           this->player->render(*this->window);
           for(auto *bullet :bullets)
           {
               bullet->render(*this->window);
           } 
           for(auto *enemy:enemies)
           {
               enemy->render(*this->window);
           }
           this->render_gui();
        }
        else if(this->state==GAME_OVER)
        {
            this->render_game_over_screen();
        }
        else if(this->state==LEADERBOARD)
        {
            this->render_leaderboard();
        }
        this->window->display();
    }

    void run()
    {
        while(this->window->isOpen())
        {
            //if(this->state==NAME_INPUT) input->handle_event(ev,*this->window);
            this->poll_events();
            if(this->player->get_hp()>0)
            this->update();
            this->render();
        }
    }

    ~Game()
    {
        delete this->window;
        delete this->player;
        for(int i=0;i<bullets.size();i++)
        {
            delete this->bullets[i];
        }
        for(auto *i:enemies)
        {
            delete i;
        }
    }
};

int main()
{
    srand(time(static_cast<unsigned>(0)));
    Game game;
    game.run();

    return 0;
}
