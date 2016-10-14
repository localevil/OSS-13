#include <iostream>
#include <string>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "UI.hpp"

using std::cout;
using std::endl;
using std::string; 

AuthUI::AuthUI(UI *ui) : ui(ui) {
	generateLoginWindow();
	generateRegistrationWindow();
}

void AuthUI::generateLoginWindow() {
	auto login_label = sfg::Label::Create("Login");
	auto passw_label = sfg::Label::Create("Password");
	auto reg_label = sfg::Label::Create("Registration");
	reg_label->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&AuthUI::openReg, this));

	auto enter_button = sfg::Button::Create("Enter");
	enter_button->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&AuthUI::enter, this));

	login_entry = sfg::Entry::Create();
	passw_entry = sfg::Entry::Create();
	login_entry->SetRequisition(sf::Vector2f(100, 0));
	passw_entry->SetRequisition(sf::Vector2f(100, 0));

	auto table = sfg::Table::Create();

	table->Attach(login_label,  sf::Rect<unsigned>(0, 0, 2, 1), NULL, sfg::Table::AttachOption::FILL,							sf::Vector2f(5, 0));
	table->Attach(passw_label,  sf::Rect<unsigned>(0, 2, 2, 1), NULL, sfg::Table::AttachOption::FILL,							sf::Vector2f(5, 0));
	table->Attach(login_entry,  sf::Rect<unsigned>(2, 0, 3, 1), sfg::Table::AttachOption::FILL, NULL,							sf::Vector2f(5, 2));
	table->Attach(passw_entry,  sf::Rect<unsigned>(2, 2, 3, 1), sfg::Table::AttachOption::FILL, NULL,							sf::Vector2f(5, 2));
	table->Attach(enter_button, sf::Rect<unsigned>(3, 3, 2, 1), sfg::Table::AttachOption::FILL, NULL,                           sf::Vector2f(0, 0));
	table->Attach(reg_label,    sf::Rect<unsigned>(5, 3, 1, 1), sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL, sf::Vector2f(5, 0));

	logWindow = sfg::Window::Create();
	logWindow->Add(table);
	logWindow->SetPosition(sf::Vector2f((ui->GetRenderWindow()->getSize().x - logWindow->GetAllocation().width) / 10,
										(ui->GetRenderWindow()->getSize().y - logWindow->GetAllocation().height) / 3));
	logWindow->SetStyle(NULL);
	ui->GetDesktop()->Add(logWindow);
}

void AuthUI::generateRegistrationWindow() {
	auto login_label = sfg::Label::Create("Input login:");
	auto passw_label = sfg::Label::Create("Input password:");

	auto reg_button = sfg::Button::Create("Registration");
	reg_button->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&AuthUI::registration, this));

	new_login_entry = sfg::Entry::Create();
	new_passw_entry = sfg::Entry::Create();
	new_login_entry->SetRequisition(sf::Vector2f(100.f, 0.f));
	new_passw_entry->SetRequisition(sf::Vector2f(100.f, 0.f));

	auto table = sfg::Table::Create();
	table->Attach(login_label, sf::Rect<unsigned>(0, 0, 5, 5), sfg::Table::AttachOption::FILL,
		sfg::Table::AttachOption::FILL, sf::Vector2f(5.f, 1.f));
	table->Attach(passw_label, sf::Rect<unsigned>(0, 5, 5, 5), sfg::Table::AttachOption::FILL,
		sfg::Table::AttachOption::FILL, sf::Vector2f(5.f, 1.f));
	table->Attach(reg_button, sf::Rect<unsigned>(3, 10, 5, 5), sfg::Table::AttachOption::FILL,
		sfg::Table::AttachOption::FILL, sf::Vector2f(5.f, 1.f));
	table->Attach(new_login_entry, sf::Rect<unsigned>(5, 0, 5, 5), sfg::Table::AttachOption::FILL,
		sfg::Table::AttachOption::FILL, sf::Vector2f(5.f, 1.f));
	table->Attach(new_passw_entry, sf::Rect<unsigned>(5, 5, 5, 5), sfg::Table::AttachOption::FILL,
		sfg::Table::AttachOption::FILL, sf::Vector2f(5.f, 1.f));

	regWindow = sfg::Window::Create();
	regWindow->Add(table);
	regWindow->SetPosition(sf::Vector2f((ui->GetRenderWindow()->getSize().x - regWindow->GetAllocation().width) / 2,
		                                (ui->GetRenderWindow()->getSize().y - regWindow->GetAllocation().height) / 2));
	regWindow->SetStyle(NULL);
	regWindow->Show(false);
	ui->GetDesktop()->Add(regWindow);
}

void AuthUI::openLogin() {
	regWindow->Show(false);
	new_login_entry->SetText("");
	new_passw_entry->SetText("");
	logWindow->Show(true);
}

void AuthUI::openReg() {
	logWindow->Show(false);
	login_entry->SetText("");
	passw_entry->SetText("");
	regWindow->Show(true);
}

void AuthUI::enter() {
	//bool enter;

	cout << string(login_entry->GetText()) << endl;
	cout << string(passw_entry->GetText()) << endl;

	Result enter = Network::send_command(LOGIN_CODE, {login_entry->GetText(), passw_entry->GetText()});
	/*if (login_entry->GetText() == "abc" && passw_entry->GetText() == "123")
		enter = true;
	else
		enter = false;*/

	if (enter == OK)
		cout << "Enter succeeded!" << endl;
	else
		cout << "Enter failed!" << endl;
}

void AuthUI::registration() {
	cout << "You have just created new login data:" << endl;
	cout << string(new_login_entry->GetText()) << endl;
	cout << string(new_passw_entry->GetText()) << endl;

	Result reg = Network::send_command(SIGNIN_CODE, { new_login_entry->GetText(), new_passw_entry->GetText() });
	if (reg == OK)
		cout << "Signin succeeded!" << endl;
	else
		cout << "Sgnin failed!" << endl;

	openLogin();
}

UI::UI(sf::RenderWindow *rendWindow) : rendWindow(rendWindow),
									   authUI(this) {
	
}

void UI::HandleEvent(sf::Event event) {
	desktop.HandleEvent(event);
}
