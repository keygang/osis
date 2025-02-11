// lab_3.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

struct Node
{
	Node *next;
	Node *prev;
	char *str;
};

Node *head = nullptr;

Node* get_first() {
	return head;
}

Node* get_last() {
	Node *temp = get_first();

	while (temp != nullptr && temp->next != nullptr) {
		temp = temp->next;
	}

	return temp;
}

void push_back(char *str) {
	Node *temp = get_last();
	Node *new_item = new Node();
	new_item->next = nullptr;
	new_item->prev = temp;
	if (temp != nullptr) {
		temp->next = new_item;
	}
	else {
		head = new_item;
	}
	new_item->str = str;
}

void push_front(char *str) {
	Node *temp = get_first();
	Node *new_item = new Node();
	new_item->next = temp;
	new_item->prev = nullptr;
	if (temp != nullptr) {
		temp->prev = new_item;
	}
	head = new_item;
	new_item->str = str;
}

bool is_empty() {
	return head == nullptr;
}

Node *get_item(int index) {
	if (is_empty()) {
		return nullptr;
	}

	int i = 0;
	Node *cur = head;
	for (i = 0; i < index && cur->next != nullptr; ++i) {
		cur = cur->next;
	}

	if (i != index) {
		return nullptr;
	}

	return cur;
}

void remove(int index) {
	Node *cur = get_item(index);
	if (cur == nullptr) {
		printf("No such item.\n");
		return;
	}

	Node *next = cur->next;
	Node *prev = cur->prev;

	if (prev != nullptr) {
		prev->next = next;
	}

	if (next != nullptr) {
		next->prev = prev;
	}

	if (cur == head) {
		head = cur->next;
	}

	delete cur;
}

void clear() {
	while (head != nullptr) {
		remove(0);
	}
}

void output() {
	Node *cur = head;
	while (cur != nullptr) {
		printf("%s\n", cur->str);
		cur = cur->next;
	}
}

int main()
{
	char cmd[16];
	int index;
	printf("Available commands: push_back 'str', push_front 'str', remove 'index', clear, output, exit\n");

	do {
		char* str = new char[256];
		scanf("%s", cmd);

		if (cmd[0] == 'p' && cmd[5] == 'b') {
			scanf("%s", str);
			push_back(str);
		}

		if (cmd[0] == 'p' && cmd[5] == 'f') {
			scanf("%s", str);
			push_front(str);
		}

		if (cmd[0] == 'r') {
			scanf("%d", &index);
			remove(index);
		}

		if (cmd[0] == 'c') {
			clear();
		}

		if (cmd[0] == 'e') {
			break;
		}

		if (cmd[0] == 'o') {
			output();
		}

	} while (true);

	return 0;
}


