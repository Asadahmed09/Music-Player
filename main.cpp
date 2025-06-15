#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <functional>
#include <iostream>
#include <filesystem>
#include <sstream>

class WindowView
{
public:
  virtual void handleEvent(const sf::Event &) = 0;
  virtual void update() = 0;
  virtual void draw() = 0;
  virtual ~WindowView() {}
};

class HomeView : public WindowView
{
  sf::RenderWindow &window;
  sf::Font &font;
  std::vector<std::string> &songs;
  std::function<void(int)> playSongCallback;

public:
  HomeView(sf::RenderWindow &win, sf::Font &f, std::vector<std::string> &s, std::function<void(int)> playCb)
      : window(win), font(f), songs(s), playSongCallback(playCb) {}
  void handleEvent(const sf::Event &event) override
  {
    if (event.type == sf::Event::MouseButtonPressed)
    {
      sf::Vector2i mousePos = sf::Mouse::getPosition(window);
      for (size_t i = 0; i < songs.size(); i++)
      {
        sf::FloatRect songBounds(220, 20 + i * 40, 350, 30);
        if (songBounds.contains(mousePos.x, mousePos.y))
        {
          playSongCallback(i);
          break;
        }
      }
    }
  }
  void update() override {}
  void draw() override
  {
    float contentStartX = 200;
    for (size_t i = 0; i < songs.size(); i++)
    {
      sf::Text text;
      text.setFont(font);
      text.setString(songs[i]);
      text.setCharacterSize(20);
      text.setFillColor(sf::Color::White);
      text.setPosition(contentStartX + 20, 20 + i * 40);
      window.draw(text);
    }
  }
};

class FavoritesView : public WindowView
{
  sf::RenderWindow &window;
  sf::Font &font;
  std::vector<std::string> &favorites;
  std::vector<std::string> &songs;
  std::function<void(int)> playSongCallback;

public:
  FavoritesView(sf::RenderWindow &win, sf::Font &f, std::vector<std::string> &fav, std::vector<std::string> &s, std::function<void(int)> cb)
      : window(win), font(f), favorites(fav), songs(s), playSongCallback(cb) {}
  void handleEvent(const sf::Event &event) override
  {
    if (event.type == sf::Event::MouseButtonPressed)
    {
      sf::Vector2i mousePos = sf::Mouse::getPosition(window);
      for (size_t i = 0; i < favorites.size(); i++)
      {
        sf::FloatRect bounds(220, 20 + i * 30, 500, 25);
        if (bounds.contains(mousePos.x, mousePos.y))
        {
          auto it = std::find(songs.begin(), songs.end(), favorites[i]);
          if (it != songs.end())
          {
            playSongCallback(std::distance(songs.begin(), it));
          }
          break;
        }
      }
    }
  }
  void update() override {}
  void draw() override
  {
    float contentStartX = 200;
    for (size_t i = 0; i < favorites.size(); i++)
    {
      sf::Text text;
      text.setFont(font);
      text.setString(favorites[i]);
      text.setCharacterSize(20);
      text.setFillColor(sf::Color::White);
      text.setPosition(contentStartX + 20, 20 + i * 30);
      window.draw(text);
    }
  }
};

class SettingsView : public WindowView
{
  sf::RenderWindow &window;
  sf::Font &font;
  sf::Text &volumeText;
  sf::RectangleShape &volumeSlider;
  std::function<void(float)> setVolumeCallback;

public:
  SettingsView(sf::RenderWindow &win, sf::Font &f, sf::Text &vt, sf::RectangleShape &vs, std::function<void(float)> cb)
      : window(win), font(f), volumeText(vt), volumeSlider(vs), setVolumeCallback(cb) {}
  void handleEvent(const sf::Event &event) override
  {
    if (event.type == sf::Event::MouseButtonPressed)
    {
      sf::Vector2i mousePos = sf::Mouse::getPosition(window);
      if (volumeSlider.getGlobalBounds().contains(mousePos.x, mousePos.y))
      {
        float newVolume = (mousePos.x - volumeSlider.getPosition().x) / volumeSlider.getSize().x * 100.0f;
        setVolumeCallback(std::max(0.0f, std::min(100.0f, newVolume)));
      }
    }
  }
  void update() override {}
  void draw() override
  {
    window.draw(volumeText);
    window.draw(volumeSlider);
  }
};

class UserView : public WindowView
{
  sf::RenderWindow &window;
  sf::Font &font;
  std::string username;
  sf::Text userText, optionsText;

public:
  UserView(sf::RenderWindow &win, sf::Font &f, const std::string &uname)
      : window(win), font(f), username(uname)
  {
    userText.setFont(font);
    userText.setCharacterSize(24);
    userText.setFillColor(sf::Color::White);
    userText.setPosition(250, 100);
    userText.setString("User: " + username);

    optionsText.setFont(font);
    optionsText.setCharacterSize(20);
    optionsText.setFillColor(sf::Color(180, 180, 180));
    optionsText.setPosition(250, 160);
    optionsText.setString("Personal Options:\n- Hey ! we are currently working on Music Player");
  }
  void handleEvent(const sf::Event &) override {}
  void update() override {}
  void draw() override
  {
    window.draw(userText);
    window.draw(optionsText);
  }
};

class User
{
public:
  std::string username;
  std::string password;
  User(const std::string &u, const std::string &p) : username(u), password(p) {}
};

class LoginView
{
  sf::RenderWindow &window;
  sf::Font &font;
  std::string usernameInput;
  std::string passwordInput;
  bool usernameActive = true;
  bool passwordActive = false;
  bool loginSuccess = false;
  bool isSignup = false;
  bool modeSelected = false;
  sf::Text promptText, userText, passText, infoText, modeText, loginBtn, signupBtn;

public:
  LoginView(sf::RenderWindow &win, sf::Font &f) : window(win), font(f)
  {
    promptText.setFont(font);
    promptText.setString("Choose Login or Signup");
    promptText.setCharacterSize(22);
    promptText.setFillColor(sf::Color::White);
    promptText.setPosition(220, 80);

    modeText.setFont(font);
    modeText.setCharacterSize(20);
    modeText.setFillColor(sf::Color::White);
    modeText.setPosition(220, 120);
    modeText.setString("Press L for Login, S for Signup");

    loginBtn.setFont(font);
    loginBtn.setCharacterSize(20);
    loginBtn.setFillColor(sf::Color::White);
    loginBtn.setPosition(220, 160);
    loginBtn.setString("[L] Login");

    signupBtn.setFont(font);
    signupBtn.setCharacterSize(20);
    signupBtn.setFillColor(sf::Color::White);
    signupBtn.setPosition(350, 160);
    signupBtn.setString("[S] Signup");

    userText.setFont(font);
    userText.setCharacterSize(20);
    userText.setFillColor(sf::Color::White);
    userText.setPosition(220, 220);

    passText.setFont(font);
    passText.setCharacterSize(20);
    passText.setFillColor(sf::Color::White);
    passText.setPosition(220, 270);

    infoText.setFont(font);
    infoText.setCharacterSize(18);
    infoText.setFillColor(sf::Color::Red);
    infoText.setPosition(220, 340);
  }
  void handleEvent(const sf::Event &event)
  {
    if (!modeSelected && event.type == sf::Event::KeyPressed)
    {
      if (event.key.code == sf::Keyboard::L)
      {
        isSignup = false;
        modeSelected = true;
        promptText.setString("Login: Enter Username and Password");
      }
      else if (event.key.code == sf::Keyboard::S)
      {
        isSignup = true;
        modeSelected = true;
        promptText.setString("Signup: Enter New Username and Password (Password: 2222)");
      }
    }
    else if (modeSelected)
    {
      if (event.type == sf::Event::TextEntered)
      {
        if (usernameActive)
        {
          if (event.text.unicode == '\b' && !usernameInput.empty())
            usernameInput.pop_back();
          else if (event.text.unicode < 128 && event.text.unicode != '\b' && event.text.unicode != '\r')
            usernameInput += static_cast<char>(event.text.unicode);
        }
        else if (passwordActive)
        {
          if (event.text.unicode == '\b' && !passwordInput.empty())
            passwordInput.pop_back();
          else if (event.text.unicode < 128 && event.text.unicode != '\b' && event.text.unicode != '\r')
            passwordInput += static_cast<char>(event.text.unicode);
        }
      }
      else if (event.type == sf::Event::KeyPressed)
      {
        if (event.key.code == sf::Keyboard::Tab)
        {
          usernameActive = !usernameActive;
          passwordActive = !passwordActive;
        }
        else if (event.key.code == sf::Keyboard::Return)
        {
          if (usernameActive)
          {
            usernameActive = false;
            passwordActive = true;
          }
          else if (passwordActive)
          {
            if (usernameInput.empty() || passwordInput.empty())
            {
              infoText.setFillColor(sf::Color::Red);
              infoText.setString("Please enter both username and password.");
              return;
            }
            std::ifstream infile("users.txt");
            std::string line, foundUser, foundPass;
            bool userExists = false;
            while (std::getline(infile, line))
            {
              std::istringstream iss(line);
              iss >> foundUser >> foundPass;
              if (foundUser == usernameInput)
              {
                userExists = true;
                break;
              }
            }
            infile.close();
            if (isSignup)
            {
              if (userExists)
              {
                infoText.setFillColor(sf::Color::Red);
                infoText.setString("Username already exists. Try another.");
              }
              else if (passwordInput != "2222")
              {
                infoText.setFillColor(sf::Color::Red);
                infoText.setString("Password must be 2222.");
              }
              else
              {
                std::ofstream outfile("users.txt", std::ios::app);
                outfile << usernameInput << " " << passwordInput << "\n";
                loginSuccess = true;
                infoText.setFillColor(sf::Color::Green);
                infoText.setString("Signup successful!");
              }
            }
            else
            {
              if (!userExists)
              {
                infoText.setFillColor(sf::Color::Red);
                infoText.setString("Username not found. Please signup.");
              }
              else if (passwordInput != "2222")
              {
                infoText.setFillColor(sf::Color::Red);
                infoText.setString("Incorrect password.");
              }
              else
              {
                loginSuccess = true;
                infoText.setFillColor(sf::Color::Green);
                infoText.setString("Login successful!");
              }
            }
          }
        }
      }
    }
  }
  void draw()
  {
    window.clear(sf::Color(30, 30, 30));
    window.draw(promptText);
    if (!modeSelected)
    {
      window.draw(modeText);
      window.draw(loginBtn);
      window.draw(signupBtn);
    }
    else
    {
      userText.setString("Username: " + usernameInput + (usernameActive ? "|" : ""));
      window.draw(userText);
      passText.setString("Password: " + std::string(passwordInput.size(), '*') + (passwordActive ? "|" : ""));
      window.draw(passText);
    }
    window.draw(infoText);
    window.display();
  }
  bool isLoggedIn() const { return loginSuccess; }
  std::string getUsername() const { return usernameInput; }
};

class MusicPlayer
{
private:
  sf::Texture backgroundTexture;
  sf::Sprite backgroundSprite;
  sf::Texture navPanelTexture;
  sf::Sprite navPanelSprite;
  sf::RenderWindow &window;
  sf::Font font;
  sf::Font modernFont;
  sf::Font extraBoldFont;
  sf::Music music;
  std::vector<std::string> songs;
  std::vector<std::string> favorites;
  float volume;
  bool isPlaying;
  int currentSongIndex;
  std::string currentWindow; // "home", "favorites", "settings"
  std::string searchQuery;
  bool searchBarActive = false;

  // UI Elements
  sf::RectangleShape navBar;
  std::vector<sf::RectangleShape> navButtons;
  std::vector<sf::Text> navTexts;
  sf::Text currentSongText;
  sf::Text volumeText;
  sf::RectangleShape volumeSlider;
  sf::RectangleShape playButton;
  sf::Text playButtonText;
  sf::RectangleShape nextButton;
  sf::Text nextButtonText;
  sf::RectangleShape prevButton;
  sf::Text prevButtonText;

  // Encapsulate player state and logic
  std::unique_ptr<WindowView> currentView;

  // Toggle button for repeat
  sf::RectangleShape repeatButton;
  sf::Text repeatButtonText;
  bool repeatOn;

  // Favourite button
  sf::RectangleShape favButton;
  sf::Text favButtonText;

  std::string username;

  int navSelectedIndex = 0;
  sf::SoundBuffer selectBuffer;
  sf::Sound selectSound;

public:
  MusicPlayer(sf::RenderWindow &win, const std::string &uname) : window(win),
                                                                 volume(100.0f),
                                                                 isPlaying(false),
                                                                 currentSongIndex(-1),
                                                                 currentWindow("home"),
                                                                 repeatOn(false),
                                                                 username(uname)
  {
    std::cout << "[DEBUG] In MusicPlayer constructor." << std::endl;
    if (!extraBoldFont.loadFromFile("Roboto_Condensed-ExtraBold.ttf"))
    {
      std::cout << "[ERROR] Could not load font Roboto_Condensed-ExtraBold.ttf!" << std::endl;
      throw std::runtime_error("Could not load Roboto Extra Bold font!");
    }
    std::cout << "[DEBUG] Font loaded successfully." << std::endl;
    font = extraBoldFont;
    modernFont = extraBoldFont;
    if (!backgroundTexture.loadFromFile("assets/bg.jpg"))
    {
      std::cout << "[ERROR] Failed to load background image.\n";
    }
    else
    {
      backgroundSprite.setTexture(backgroundTexture);
      backgroundSprite.setScale(
          (float)window.getSize().x / backgroundTexture.getSize().x,
          (float)window.getSize().y / backgroundTexture.getSize().y);
    }
    if (!navPanelTexture.loadFromFile("assets/logo.jpg"))
    {
      std::cout << "[ERROR] Could not load navigation panel background image.\n";
    }
    else
    {
      navPanelSprite.setTexture(navPanelTexture);

      // Scale image to match nav bar size: 200x600
      navPanelSprite.setScale(
          200.f / navPanelTexture.getSize().x,
          600.f / navPanelTexture.getSize().y);

      navPanelSprite.setPosition(0, 0); // top-left
    }

    setupUI();
    std::cout << "[DEBUG] UI setup complete." << std::endl;
    loadSongs();
    std::cout << "[DEBUG] Songs loaded: " << songs.size() << std::endl;
    loadFavorites();
    std::cout << "[DEBUG] Favorites loaded: " << favorites.size() << std::endl;
    switchView("home");
    std::cout << "[DEBUG] Initial view set to home." << std::endl;

    if (selectBuffer.loadFromFile("select.wav"))
    {
      selectSound.setBuffer(selectBuffer);
    }
  }

  void handleEvent(const sf::Event &event)
  {
    // Arrow key navigation for nav bar
    if (event.type == sf::Event::KeyPressed)
    {
      int navCount = navButtons.size();
      if (event.key.code == sf::Keyboard::Down)
      {
        navSelectedIndex = (navSelectedIndex + 1) % navCount;
        selectSound.play();
      }
      else if (event.key.code == sf::Keyboard::Up)
      {
        navSelectedIndex = (navSelectedIndex - 1 + navCount) % navCount;
        selectSound.play();
      }
      else if (event.key.code == sf::Keyboard::Enter)
      {
        switch (navSelectedIndex)
        {
        case 0:
          switchView("home");
          break;
        case 1:
          switchView("favorites");
          break;
        case 2:
          switchView("settings");
          break;
        case 3:
          switchView("user");
          break;
        }
        selectSound.play();
      }
    }
    if (event.type == sf::Event::MouseButtonPressed)
    {
      // Mouse navigation still works
      sf::Vector2i mousePos = sf::Mouse::getPosition(window);
      for (size_t i = 0; i < navButtons.size(); i++)
      {
        if (navButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y))
        {
          navSelectedIndex = i;
          selectSound.play();
          switch (i)
          {
          case 0:
            switchView("home");
            break;
          case 1:
            switchView("favorites");
            break;
          case 2:
            switchView("settings");
            break;
          case 3:
            switchView("user");
            break;
          }
          // Do not return here; allow other UI elements to process the event
        }
      }
    }
    if (event.type == sf::Event::MouseButtonPressed && currentWindow == "home")
    {
      float contentStartX = 200;
      float contentWidth = 800;
      float searchBarWidth = 400;
      float searchBarX = contentStartX + (contentWidth - searchBarWidth) / 2;
      sf::FloatRect searchBarRect(searchBarX, 10, searchBarWidth, 30);
      sf::Vector2i mousePos = sf::Mouse::getPosition(window);
      if (searchBarRect.contains(mousePos.x, mousePos.y))
      {
        searchBarActive = true;
      }
      else
      {
        searchBarActive = false;
      }
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
    {
      searchBarActive = false;
    }
    if (event.type == sf::Event::TextEntered && currentWindow == "home" && searchBarActive)
    {
      if (event.text.unicode == '\b' && !searchQuery.empty())
        searchQuery.pop_back();
      else if (event.text.unicode < 128 && event.text.unicode != '\b' && event.text.unicode != '\r')
        searchQuery += static_cast<char>(event.text.unicode);
    }
    if (playButton.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y) && event.type == sf::Event::MouseButtonPressed)
    {
      togglePlay();
    }
    else if (nextButton.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y) && event.type == sf::Event::MouseButtonPressed)
    {
      playNext();
    }
    else if (prevButton.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y) && event.type == sf::Event::MouseButtonPressed)
    {
      playPrevious();
    }
    else if (isPlaying && repeatButton.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y) && event.type == sf::Event::MouseButtonPressed)
    {
      repeatOn = !repeatOn;
      repeatButtonText.setString(repeatOn ? "Repeat: ON" : "Repeat: OFF");
    }
    else if (isPlaying && favButton.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y) && event.type == sf::Event::MouseButtonPressed)
    {
      if (isCurrentSongFavourite())
      {
        removeFromFavorites(currentSongIndex);
      }
      else
      {
        addToFavorites(currentSongIndex);
      }
      updateFavButton();
    }
    if (currentView)
      currentView->handleEvent(event);
  }

  void update()
  {
    // Update music status
    if (isPlaying && music.getStatus() == sf::Music::Stopped)
    {
      if (repeatOn)
      {
        playSong(currentSongIndex);
      }
      else
      {
        playNext();
      }
    }
    if (currentView)
      currentView->update();
  }

  void draw()
  {
    window.clear();
    window.draw(backgroundSprite); // Draw background first

    // Draw navigation bar
    window.draw(navPanelSprite); // Draw the background image instead
    for (size_t i = 0; i < navButtons.size(); i++)
    {
      if ((int)i == navSelectedIndex)
      {
        navButtons[i].setFillColor(sf::Color(120, 120, 200));
      }
      else
      {
        navButtons[i].setFillColor(sf::Color(70, 70, 70));
      }
      window.draw(navButtons[i]);
    }
    for (const auto &text : navTexts)
    {
      window.draw(text);
    }

    // Draw music controls
    window.draw(playButton);
    window.draw(playButtonText);
    window.draw(nextButton);
    window.draw(nextButtonText);
    window.draw(prevButton);
    window.draw(prevButtonText);
    window.draw(currentSongText);

    // Draw content based on current window
    if (currentWindow == "home")
    {
      drawSongList(songs);
    }
    else if (currentWindow == "favorites")
    {
      drawSongList(favorites);
    }
    else if (currentWindow == "settings")
    {
      window.draw(volumeText);
      window.draw(volumeSlider);
    }
    else if (currentWindow == "user")
    {
      currentView = std::make_unique<UserView>(window, extraBoldFont, username);
      currentView->draw();
    }

    if (isPlaying)
    {
      window.draw(favButton);
      window.draw(favButtonText);
      window.draw(repeatButton);
      window.draw(repeatButtonText);
    }

    window.display();
  }

  void switchView(const std::string &viewName)
  {
    if (viewName == "home")
    {
      currentView = std::make_unique<HomeView>(window, extraBoldFont, songs, [this](int i)
                                               { playSong(i); });
      currentWindow = "home";
    }
    else if (viewName == "favorites")
    {
      currentView = std::make_unique<FavoritesView>(window, extraBoldFont, favorites, songs, [this](int i)
                                                    { playSong(i); });
      currentWindow = "favorites";
    }
    else if (viewName == "settings")
    {
      currentView = std::make_unique<SettingsView>(window, extraBoldFont, volumeText, volumeSlider, [this](float v)
                                                   { setVolume(v); });
      currentWindow = "settings";
    }
    else if (viewName == "user")
    {
      currentView = std::make_unique<UserView>(window, extraBoldFont, username);
      currentWindow = "user";
    }
  }

private:
  void setupUI()
  {
    // Navigation bar
    navBar.setSize(sf::Vector2f(200, 600));
    navBar.setFillColor(sf::Color(50, 50, 50));
    navBar.setPosition(0, 0);

    // Navigation buttons
    std::vector<std::string> buttonTexts = {"Home", "Favorites", "Settings", "User"};
    float navButtonHeight = 50;
    float navSpacing = 10;

    for (size_t i = 0; i < buttonTexts.size(); i++)
    {
      sf::RectangleShape button;
      button.setSize(sf::Vector2f(180, navButtonHeight));
      button.setPosition(10, 10 + i * (navButtonHeight + navSpacing));
      button.setFillColor(sf::Color(70, 70, 70));
      navButtons.push_back(button);

      sf::Text text;
      text.setFont(extraBoldFont);
      text.setString(buttonTexts[i]);
      text.setCharacterSize(20);
      text.setFillColor(sf::Color::White);
      text.setPosition(20, 20 + i * (navButtonHeight + navSpacing));
      navTexts.push_back(text);
    }

    // Center controls in content area (right side)
    float contentWidth = 800;
    float contentStartX = 200; // left nav width
    float controlsY = 400;     // vertical position for controls
    float ctrlButtonWidth = 100;
    float ctrlButtonHeight = 40;
    float ctrlSpacing = 30;
    float totalWidth = ctrlButtonWidth * 3 + ctrlSpacing * 2;
    float startX = contentStartX + (contentWidth - totalWidth) / 2;

    // Use a consistent dew green color
    sf::Color dewGreen(34, 197, 94);

    // === PREV Button ===
    prevButton.setSize(sf::Vector2f(100, 35));
    prevButton.setPosition(startX, controlsY);
    prevButton.setFillColor(dewGreen);
    prevButton.setOutlineColor(sf::Color::White);
    prevButton.setOutlineThickness(2.f);

    prevButtonText.setFont(extraBoldFont);
    prevButtonText.setString("Prev");
    prevButtonText.setCharacterSize(20);
    prevButtonText.setFillColor(sf::Color::White);
    prevButtonText.setPosition(startX + 20, controlsY + 6);

    // === PLAY Button ===
    playButton.setSize(sf::Vector2f(100, 35));
    playButton.setPosition(startX + ctrlButtonWidth + ctrlSpacing, controlsY);
    playButton.setFillColor(dewGreen);
    playButton.setOutlineColor(sf::Color::White);
    playButton.setOutlineThickness(2.f);

    playButtonText.setFont(extraBoldFont);
    playButtonText.setString("Play");
    playButtonText.setCharacterSize(20);
    playButtonText.setFillColor(sf::Color::White);
    playButtonText.setPosition(startX + ctrlButtonWidth + ctrlSpacing + 20, controlsY + 6);

    // === NEXT Button ===
    nextButton.setSize(sf::Vector2f(100, 35));
    nextButton.setPosition(startX + 2 * (ctrlButtonWidth + ctrlSpacing), controlsY);
    nextButton.setFillColor(dewGreen);
    nextButton.setOutlineColor(sf::Color::White);
    nextButton.setOutlineThickness(2.f);

    nextButtonText.setFont(extraBoldFont);
    nextButtonText.setString("Next");
    nextButtonText.setCharacterSize(20);
    nextButtonText.setFillColor(sf::Color::White);
    nextButtonText.setPosition(startX + 2 * (ctrlButtonWidth + ctrlSpacing) + 20, controlsY + 6);

    // === Song Text ===
    currentSongText.setFont(extraBoldFont);
    currentSongText.setString("No song playing");
    currentSongText.setCharacterSize(20);
    currentSongText.setFillColor(sf::Color::White);
    currentSongText.setPosition(contentStartX + 50, controlsY - 50);

    // Volume control
    volumeText.setFont(extraBoldFont);
    volumeText.setString("Volume: 100%");
    volumeText.setCharacterSize(20);
    volumeText.setFillColor(sf::Color::White);
    volumeText.setPosition(220, 20);

    volumeSlider.setSize(sf::Vector2f(200, 20));
    volumeSlider.setPosition(220, 50);
    volumeSlider.setFillColor(sf::Color(100, 100, 100));

    // Repeat toggle button
    repeatButton.setSize(sf::Vector2f(140, 40));
    repeatButton.setFillColor(sf::Color(70, 130, 180));
    repeatButton.setPosition(playButton.getPosition().x + 120, playButton.getPosition().y + 60);
    repeatButtonText.setFont(extraBoldFont);
    repeatButtonText.setString("Repeat: OFF");
    repeatButtonText.setCharacterSize(18);
    repeatButtonText.setFillColor(sf::Color::White);
    repeatButtonText.setPosition(repeatButton.getPosition().x + 10, repeatButton.getPosition().y + 8);

    // Favourite button (to the left of repeat)
    favButton.setSize(sf::Vector2f(140, 40));
    favButton.setPosition(playButton.getPosition().x - 170, playButton.getPosition().y + 60);
    favButton.setFillColor(sf::Color(200, 200, 200));
    favButtonText.setFont(extraBoldFont);
    favButtonText.setString("Favourite");
    favButtonText.setCharacterSize(18);
    favButtonText.setFillColor(sf::Color::Black);
    favButtonText.setPosition(favButton.getPosition().x + 10, favButton.getPosition().y + 8);
  }

  void loadSongs()
  {
    songs = {"Music1.ogg", "Music2.ogg", "Music3.ogg", "cold.mp3"};
  }

  void loadFavorites()
  {
    std::ifstream file("favorites.txt");
    std::string song;
    while (std::getline(file, song))
    {
      favorites.push_back(song);
    }
  }

  void drawSongList(const std::vector<std::string> &songList)
  {
    float contentStartX = 200;
    float contentWidth = 800;
    float searchBarWidth = 400;
    float searchBarX = contentStartX + (contentWidth - searchBarWidth) / 2;
    // Draw search bar
    sf::RectangleShape searchBar;
    searchBar.setSize(sf::Vector2f(searchBarWidth, 30));
    searchBar.setPosition(searchBarX, 10);
    searchBar.setFillColor(searchBarActive ? sf::Color(80, 80, 120) : sf::Color(50, 50, 50));
    window.draw(searchBar);
    sf::Text searchText;
    searchText.setFont(extraBoldFont);
    searchText.setString("Search: " + searchQuery + (searchBarActive ? "|" : ""));
    searchText.setCharacterSize(18);
    searchText.setFillColor(sf::Color::White);
    searchText.setPosition(searchBarX + 10, 13);
    window.draw(searchText);
    // Filter and draw songs
    int shown = 0;
    for (size_t i = 0; i < songList.size(); i++)
    {
      if (searchQuery.empty() || songList[i].find(searchQuery) != std::string::npos)
      {
        sf::Text text;
        text.setFont(extraBoldFont);
        text.setString(songList[i]);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::White);
        text.setPosition(contentStartX + 20, 50 + shown * 40);
        window.draw(text);
        shown++;
      }
    }
  }

  void playSong(int index)
  {
    if (index >= 0 && index < songs.size())
    {
      currentSongIndex = index;
      music.stop();
      if (music.openFromFile(songs[index]))
      {
        music.setVolume(volume);
        music.play();
        isPlaying = true;
        currentSongText.setString("Now playing: " + songs[index]);
        playButtonText.setString("Pause");
        updateFavButton();
      }
    }
  }

  void togglePlay()
  {
    if (currentSongIndex >= 0)
    {
      if (isPlaying)
      {
        music.pause();
        isPlaying = false;
        playButtonText.setString("Play");
      }
      else
      {
        music.play();
        isPlaying = true;
        playButtonText.setString("Pause");
      }
    }
  }

  void playNext()
  {
    if (!songs.empty())
    {
      int nextIndex = (currentSongIndex + 1) % songs.size();
      playSong(nextIndex);
    }
  }

  void playPrevious()
  {
    if (!songs.empty())
    {
      int prevIndex = (currentSongIndex - 1 + songs.size()) % songs.size();
      playSong(prevIndex);
    }
  }

  void setVolume(float newVolume)
  {
    volume = newVolume;
    music.setVolume(volume);
    volumeText.setString("Volume: " + std::to_string((int)volume) + "%");
  }

  void addToFavorites(int songIndex)
  {
    if (songIndex >= 0 && songIndex < (int)songs.size())
    {
      const std::string &song = songs[songIndex];
      if (std::find(favorites.begin(), favorites.end(), song) == favorites.end())
      {
        favorites.push_back(song);
        std::ofstream file("favorites.txt");
        for (const auto &fav : favorites)
          file << fav << "\n";
      }
    }
  }

  void removeFromFavorites(int songIndex)
  {
    if (songIndex >= 0 && songIndex < (int)songs.size())
    {
      const std::string &song = songs[songIndex];
      auto it = std::find(favorites.begin(), favorites.end(), song);
      if (it != favorites.end())
      {
        favorites.erase(it);
        std::ofstream file("favorites.txt");
        for (const auto &fav : favorites)
          file << fav << "\n";
      }
    }
  }

  bool isCurrentSongFavourite()
  {
    if (currentSongIndex >= 0 && currentSongIndex < (int)songs.size())
    {
      return std::find(favorites.begin(), favorites.end(), songs[currentSongIndex]) != favorites.end();
    }
    return false;
  }

  void updateFavButton()
  {
    if (isCurrentSongFavourite())
    {
      favButton.setFillColor(sf::Color(255, 215, 0)); // Gold
      favButtonText.setString("Added Favourite");
      favButtonText.setFillColor(sf::Color::Black);
    }
    else
    {
      favButton.setFillColor(sf::Color(200, 200, 200)); // Light gray
      favButtonText.setString("Favourite");
      favButtonText.setFillColor(sf::Color::Black);
    }
  }
};

int main()
{
  std::cout << "[DEBUG] Top of main reached." << std::endl;
  try
  {
    sf::RenderWindow window(sf::VideoMode(1000, 600), "SFML Music Player");
    window.setFramerateLimit(60);
    sf::Font font;
    font.loadFromFile("Roboto_Condensed-ExtraBold.ttf");
    LoginView login(window, font);
    while (window.isOpen() && !login.isLoggedIn())
    {
      sf::Event event;
      while (window.pollEvent(event))
      {
        if (event.type == sf::Event::Closed)
          window.close();
        login.handleEvent(event);
      }
      login.draw();
    }
    if (!window.isOpen())
      return 0;
    std::cout << "[DEBUG] Creating MusicPlayer..." << std::endl;
    MusicPlayer player(window, login.getUsername());
    std::cout << "[DEBUG] MusicPlayer created successfully." << std::endl;
    while (window.isOpen())
    {
      sf::Event event;
      while (window.pollEvent(event))
      {
        if (event.type == sf::Event::Closed)
          window.close();
        player.handleEvent(event);
      }
      player.update();
      player.draw();
    }
  }
  catch (const std::exception &ex)
  {
    std::cout << "[ERROR] Exception: " << ex.what() << std::endl;
    system("pause");
  }
  return 0;
}
