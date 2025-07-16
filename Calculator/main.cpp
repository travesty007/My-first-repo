#include <SFML/Graphics.hpp>
#include <string>
#include <stack>
#include <deque>

sf::Font font;

static const char my_chars[] = { 
    '0', '1', '2',
    '3', '4', '5',
    '6', '7', '8',
    '9', '-', '+',
    '/', '.', '*'
};

void makeButton(sf::RenderWindow& window, std::string str, double sizeX, double sizeY, double posX, double posY, double textPosX, double textPosY, int r, int g, int b) {
    sf::RectangleShape ZeroButton(sf::Vector2f(sizeX, sizeY));
    ZeroButton.setPosition(posX, posY);
    ZeroButton.setFillColor(sf::Color(r, g, b));
    sf::Text ZeroText;
    ZeroText.setFont(font);
    ZeroText.setString(str);
    ZeroText.setCharacterSize(40);
    ZeroText.setStyle(sf::Text::Bold);
    ZeroText.setPosition(textPosX, textPosY);
    ZeroText.setFillColor(sf::Color(13, 13, 13));
    window.draw(ZeroButton);
    window.draw(ZeroText);
}

std::string evaluate(std::string expression) {
    if (expression == "") return "";
    std::deque<std::string> RPN;
    std::stack<char> RPNStack;
    std::stack<std::string> answerStack;
    std::string temp("");
    bool minusSign = true;

    for (char ch : expression) {
        if (isdigit(ch)) {
            temp += ch;
            minusSign = 0;
        }
        else if (ch == '.') {
            temp += ch;
            minusSign = 0;
        }
        else if (ch == '(') {
            if (temp != "") RPN.push_back(temp);
            temp = "";
            RPNStack.push(ch);
            minusSign = 1;
        }
        else if (ch == ')') {
            if (temp != "") RPN.push_back(temp);
            while (RPNStack.top() != '(') {
                temp = RPNStack.top();
                if (temp != "") RPN.push_back(temp);
                RPNStack.pop();
                temp = "";
            }
            RPNStack.pop();
            minusSign = 0;
        }
        else {
            if (temp != "") RPN.push_back(temp);
            if (ch == '-' or ch == '+') {
                if (minusSign) {
                    temp += ch;
                }
                else {
                    while (!RPNStack.empty() and RPNStack.top() != '(') {
                        temp = RPNStack.top();
                        if (temp != "") RPN.push_back(temp);
                        RPNStack.pop();
                    }
                }
            }
            else {
                while (!RPNStack.empty() and (RPNStack.top() == '*' or RPNStack.top() == '/')) {
                    temp = RPNStack.top();
                    if (temp != "") RPN.push_back(temp);
                    RPNStack.pop();
                }
            }
            if (ch != '-' or !minusSign) {
                temp = "";
                RPNStack.push(ch);
            }
            minusSign = 0;
        }
    }
    if (temp != "") RPN.push_back(temp);
    temp = "";

    while (!RPNStack.empty()) {
        temp = RPNStack.top();
        RPN.push_back(temp);
        RPNStack.pop();
    }

    while (!RPN.empty()) {
        temp = RPN.front();
        long long dotCount = 0;
        for (auto ch : temp) {
            if (ch == '.') dotCount++;
        }
        if (dotCount > 1 or temp[0] == '.' or temp.back() == '.') {
            answerStack.push("Error");
            break;
        }

        if ((temp.length() == 1 and isdigit(temp[0])) or temp.length() > 1) {
            answerStack.push(temp);
        }
        else {
            if (answerStack.size() < 2) {
                answerStack.push("Error");
                break;
            }
            double num2 = stod(answerStack.top());
            answerStack.pop();
            double num1 = stod(answerStack.top());
			answerStack.pop();
            double res;

			if (temp == "+") {
                res = num1 + num2;
                if (res == std::trunc(res))
                    answerStack.push(std::to_string((long long)std::trunc(res)));
                else
                    answerStack.push(std::to_string(res));
			}
            else if (temp == "-") {
                res = num1 - num2;
                if (res == std::trunc(res))
                    answerStack.push(std::to_string((long long)std::trunc(res)));
                else
                    answerStack.push(std::to_string(res));
            }
            else if (temp == "*") {
                res = num1 * num2;
                if (res == std::trunc(res))
                    answerStack.push(std::to_string((long long)std::trunc(res)));
                else
                    answerStack.push(std::to_string(res));
            }
            else {
                if (num2 == 0) {
                    answerStack.push("Error");
                    break;
                }
                res = num1 / num2;
                if (res == std::trunc(res))
                    answerStack.push(std::to_string((long long)std::trunc(res)));
                else
                    answerStack.push(std::to_string(res));
            }
        }
        RPN.pop_front();
    }

    return answerStack.top();
}

int main() {
    int size = 100;
    std::string typedExpression("");
    sf::RenderWindow window(sf::VideoMode(400, 588), "Calculator", sf::Style::Titlebar | sf::Style::Close);

    window.setKeyRepeatEnabled(false);

    font.loadFromFile("font.ttf");
    auto image = sf::Image{};
    image.loadFromFile("icon.png");
    window.setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());

    // run the program as long as the window is open
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::TextEntered) {
                bool typable = false;
                for (auto ch : my_chars) if (ch == static_cast<char>(event.text.unicode)) typable = true;
                if (typable) {
                    if (typedExpression == "Error") typedExpression = "";
                    typedExpression += static_cast<char>(event.text.unicode);
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                if (typedExpression == "Error") typedExpression = "";
                if (event.key.code == sf::Keyboard::Key::Equal and event.key.shift == 0 or event.key.code == sf::Keyboard::Key::Enter) {
                    typedExpression = evaluate(typedExpression);
                }
                if (event.key.code == sf::Keyboard::Key::BackSpace) {
                    if (typedExpression != "") typedExpression.pop_back();
                }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (typedExpression == "Error") typedExpression = "";
					sf::Vector2i localPos = sf::Mouse::getPosition(window);
					long long x = localPos.x;
					long long y = localPos.y;
					if (15 < x and x < 200 and 475 < y and y < 565) typedExpression += "0";
                    if (205 < x and x < 295 and 475 < y and y < 565) typedExpression += ".";
                    if (300 < x and x < 385 and 380 < y and y < 565) typedExpression = evaluate(typedExpression);
                    if (300 < x and x < 385 and 285 < y and y < 375) typedExpression += "*";
                    if (300 < x and x < 385 and 190 < y and y < 280) typedExpression += "/";
                    if (300 < x and x < 385 and 125 < y and y < 185) typedExpression += "+";
                    if (205 < x and x < 295 and 125 < y and y < 185) typedExpression += "-";
                    if (110 < x and x < 200 and 125 < y and y < 185 and typedExpression != "") typedExpression.pop_back();
                    if (15 < x and x < 105 and 125 < y and y < 185) typedExpression = "";
                    if (15 < x and x < 105 and 190 < y and y < 280) typedExpression += "1";
                    if (110 < x and x < 200 and 190 < y and y < 280) typedExpression += "2";
                    if (205 < x and x < 295 and 190 < y and y < 280) typedExpression += "3";
                    if (15 < x and x < 105 and 285 < y and y < 375) typedExpression += "4";
                    if (110 < x and x < 200 and 285 < y and y < 375) typedExpression += "5";
                    if (205 < x and x < 295 and 285 < y and y < 375) typedExpression += "6";
                    if (15 < x and x < 105 and 380 < y and y < 470) typedExpression += "7";
                    if (110 < x and x < 200 and 380 < y and y < 470) typedExpression += "8";
                    if (205 < x and x < 295 and 380 < y and y < 470) typedExpression += "9";
				}
            }
        }

        window.clear(sf::Color(197, 142, 190));

        if (typedExpression.size() != 0) size = 370 / typedExpression.size();
        if (size >= 50) size = 49;

        sf::Text expressionText;
        expressionText.setFont(font);
        expressionText.setString(typedExpression);
        expressionText.setCharacterSize(size * 20 / 11);
        expressionText.setStyle(sf::Text::Bold);
        expressionText.setPosition(10, 20);
        expressionText.setFillColor(sf::Color(13, 13, 13));
        window.draw(expressionText);

        for (int i = 190; i <= 465; i += 95) {
            for (int j = 15; j <= 215; j += 95) {
                makeButton(window, std::to_string((j - 15) / 95 + 3 * (i - 190) / 95 + 1), 90, 90, j, i, j + 34, i + 22, 237, 183, 207);
            }
        }

        makeButton(window, "0", 185, 90, 15, 475, 94, 497, 237, 183, 207);
        makeButton(window, ".", 90, 90, 205, 475, 245, 497, 114, 233, 247);
        makeButton(window, "=", 85, 185, 300, 380, 332, 435, 114, 233, 247);
        makeButton(window, "*", 85, 90, 300, 285, 334, 310, 114, 233, 247);
        makeButton(window, "/", 85, 90, 300, 190, 332, 214, 114, 233, 247);
        makeButton(window, "+", 85, 60, 300, 125, 332, 131, 114, 233, 247);
        makeButton(window, "-", 90, 60, 205, 125, 243, 131, 114, 233, 247);
        makeButton(window, "<-", 90, 60, 110, 125, 136, 127, 114, 233, 247);
        makeButton(window, "C", 90, 60, 15, 125, 50, 131, 114, 233, 247);


        // end the current frame
        window.display();
    }

    return 0;
}