#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

const int MAX_SIZE = 100;

struct Patient {
	int cardNumber;
	int diseaseCode;
	char doctorLastName[MAX_SIZE];
};

struct HashTableElement { // ������ ������������
	int key = -1;
	int recordIndex = -1;
	HashTableElement* next = nullptr;
};

class HashTable {
private:
	HashTableElement* table;
	int size;
	int cnt;
public:
	HashTable(int size) {
		this->size = size;
		table = new HashTableElement[size];
		for (int i = 0; i < size - 1; i++)
			table[i].next = &table[i + 1];
	}

	int hashFunction(int key, int size) {
		int hash = key;
		hash = hash * hash * 3;
		hash = hash >> key % 5;
		hash = hash + key % 8 - key % 4;
		hash = hash + (key << 2);
		hash = hash / 5;
		hash = hash * hash * hash;
		hash = hash % size;
		return abs(hash);
	}

	void insert(int key, int recordIndex) { // ������� ������ �������� � ���-�������
		if (cnt == size) {
			reHash();
		}
		int hashValue = hashFunction(key, size); // �������� ����� / �� ���-�� ���������
		HashTableElement* current = &table[hashValue];  // ����� ������ �� �������
		while (current->key != -1) { // ���� �� �� �������� ��������� ���-�������
			if (current->next == nullptr) { // ���� ��� ���������
				reHash(key, recordIndex);
				current->next = new HashTableElement; // ��������� ������� � ����� ������
			}
			current = current->next; // ������� �� ��������� ������� next->next->...
		}
		cnt++;
		current->key = key; // ��������� ��������� ���� ���-�������
		current->recordIndex = recordIndex;
	}

	int lookup(int key) {
		int hashValue = hashFunction(key, size);
		HashTableElement* current = &table[hashValue];
		while (current->key != key) {
			if (current->next == nullptr) {
				return -1;
			}
			current = current->next;
		}
		return current->recordIndex;
	}

	int remove(int key) {
		int hashValue = hashFunction(key, size);
		HashTableElement* current = &table[hashValue];
		while (current->key != key) {
			if (current->next == nullptr) {
				return -1;
			}
			current = current->next;
		}
		current->key = -1;
		current->recordIndex = -1;
	}

	void reHash(int key, int recordIndex) {
		cout << "����� reHash()" << endl;
		HashTableElement* current = table;
		int newSize = size * 2;
		HashTableElement* newTable = new HashTableElement[newSize];
		for (int i = 0; i < newSize - 1; i++)
			newTable[i].next = &newTable[i + 1];
		for (int i = 0; i < size; i++) {
			if (table[i].key != -1) {
				//cout << table[i].recordIndex << "\t" << table[i].key << endl;
				int hashValue = hashFunction(table[i].key, newSize);
				HashTableElement* current = &newTable[hashValue];
				while (current->key != -1) {
					current = current->next;
				}
				current->key = table[i].key;
				current->recordIndex = table[i].recordIndex;
			}
		}
		delete[] table;
		table = newTable;
		size = newSize;
		insert(key, recordIndex);
	}

	void reHash() {
		cout << "����� reHash()" << endl;
		HashTableElement* current = table;
		int newSize = size * 2;
		HashTableElement* newTable = new HashTableElement[newSize];
		for (int i = 0; i < newSize - 1; i++)
			newTable[i].next = &newTable[i + 1];
		for (int i = 0; i < size; i++) {
			if (table[i].key != -1) {
				//cout << table[i].recordIndex << "\t" << table[i].key << endl;
				int hashValue = hashFunction(table[i].key, newSize);
				HashTableElement* current = &newTable[hashValue];
				while (current->key != -1) {
					current = current->next;
				}
				current->key = table[i].key;
				current->recordIndex = table[i].recordIndex;
			}
		}
		delete[] table;
		table = newTable;
		size = newSize;
	}
};

class BinaryFileManager {
private:
	string sourceFile;
	string destinationFile;
public:

	BinaryFileManager(string sourceFile, string destinationFile)
		: sourceFile(sourceFile), destinationFile(destinationFile) {
		toBinary();
	}

	string getPathFile() {
		return destinationFile;
	}

	string setPathFile(const string& filename) {
		sourceFile = filename;
	}

	void toBinary() {
		ifstream inputFile(sourceFile);
		ofstream outputFile(destinationFile, ios::out | ios::trunc | ios::binary);
		Patient patient;
		if (inputFile.is_open()) {
			while (inputFile >> patient.cardNumber) {
				inputFile.ignore();
				inputFile.getline(patient.doctorLastName, sizeof(patient.doctorLastName));
				inputFile >> patient.diseaseCode;

				outputFile.write(reinterpret_cast<char*>(&patient), sizeof(Patient));
			}
			outputFile.close();
			inputFile.close();
			cout << "���� ������� ������." << endl;
		}
		else {
			cout << "������ �������� �����." << endl;
		}
	}

	void insert(Patient patient) {
		fstream file;
		bool flag = true;
		file.open(destinationFile, ios::in | ios::out | ios::binary | ios::app);
		
		if (file.is_open()) {
			file.write(reinterpret_cast<const char*>(&patient), sizeof(Patient));
			file.close();
			cout << "������ ������� ���������." << endl;
		}
		else {
			cout << "������ �������� �����." << endl;
		}
	}

	void remove(int key) {
		fstream file;
		bool flag = true;
		file.open(destinationFile, ios::in | ios::out | ios::binary);

		if (file.is_open()) {
			Patient patient;
			file.seekg(0, ios::end);
			int totalPatients = file.tellg() / sizeof(Patient);

			for (int i = 0; i < totalPatients; i++) {
				file.seekg(i * sizeof(Patient));
				file.read((char*)&patient, sizeof(Patient));

				if (patient.cardNumber == key) {
					file.seekg(-1 * sizeof(Patient), ios::end);
					Patient lastPatient;
					file.read((char*)&lastPatient, sizeof(Patient));
					file.seekp(i * sizeof(Patient));
					file.write((char*)&lastPatient, sizeof(Patient));
					flag = false;
					break;
				}
			}
			if (flag) {
				cout << "������ �� �������." << endl;
			}
			else {
				cout << "������ ������� �������." << endl;
			}
			file.close();
		}
		else {
			cout << "������ �������� �����." << endl;
		}
	}


	void lookup(int recordIndex) {
		ifstream file(destinationFile, ios::binary);
		if (file.is_open()) {

			file.seekg((recordIndex - 1) * sizeof(Patient));

			Patient patient;

			if (file.read(reinterpret_cast<char*>(&patient), sizeof(Patient))) {

				cout << "����� �������� ��������: " << patient.cardNumber << endl;
				cout << "��� ������������ �����������: " << patient.diseaseCode << endl;
				cout << "������� �������� �����: " << patient.doctorLastName << endl;

				file.close();
			}
			else {
				cout << "������ ������ ������." << endl;
			}
		}
		else {
			cout << "������ �������� �����." << endl;
		}
	}

};

class HashedBinaryFileManager {
private:
	BinaryFileManager binaryFileManager;
	HashTable hashTable;
public:
	HashedBinaryFileManager(string sourceFile, string destinationFile, int tableSize) 
		: binaryFileManager(sourceFile, destinationFile), hashTable(tableSize) {}

	int getRecordCount() {
		ifstream file(binaryFileManager.getPathFile(), ios::binary | ios::ate);
		if (file.is_open()) {
			int fileSize = file.tellg();
			file.close();
			return fileSize / sizeof(Patient);
		}
		else {
			cout << "������ �������� �����." << endl;
			return 0;
		}
	}

	void insert(const Patient& patient) {
		binaryFileManager.insert(patient);
		int recordIndex = getRecordCount();
		hashTable.insert(patient.cardNumber, recordIndex);
	}
	
	void lookup(int cardNumber) {
		int recordIndex = hashTable.lookup(cardNumber);
		binaryFileManager.lookup(recordIndex);
	}

	void remove(int cardNumber) {
		if (hashTable.lookup(cardNumber) != getRecordCount()) {
			hashTable.remove(cardNumber);
			binaryFileManager.remove(cardNumber);
		}
		else if (getRecordCount() == 1 || getRecordCount() == 0)
			cout << "�������� ����� �������." << endl;
		else
			cout << "������ ������� ��������� ������" << endl;
	}
};

int main() {
	setlocale(LC_ALL, "russian");
	int fileSize = 5;
	BinaryFileManager file("basicdata.txt", "binarytest.bin");
	HashTable hash(fileSize);
	HashedBinaryFileManager final("basichash.txt", "binarydata.bin", 5);
	cout << endl;
	Patient patient;
	int temp = 0;
	int choose = 0;
	while (true) {
		cout << "�������� �������� � �������:" << endl;
		cout << "0. ����� �� ���������;" << endl;
		cout << "1. ������� ������ ��������;" << endl;
		cout << "2. �������� �������� �� �����;" << endl;
		cout << "3. ����� �������� �� �����;" << endl;
		cout << "4. �������� ������;" << endl << ">> ";
		cin >> choose;
		switch (choose) {
		case 0: 
			cout << endl << "����� �� ���������." << endl;
			return 0;
			break;
		case 1:
			cout << endl << "������� ����� �������� ��������:" << endl << ">> ";
			cin >> patient.cardNumber;
			cout << "������� ��� ������������ �����������:" << endl << ">> ";
			cin >> patient.diseaseCode;
			cout << "������� ��� �������� �����:" << endl << ">> ";
			cin.ignore();
			cin.getline(patient.doctorLastName, 100);
			cout << endl;
			final.insert(patient);
			cout << endl;
			break;
		case 2:
			cout << endl << "������� ����� �������� ��������:" << endl << ">> ";
			cin >> temp;
			cout << endl;
			final.remove(temp);
			cout << endl;
			break;
		case 3:
			cout << endl << "������� ����� �������� ��������:" << endl << ">> ";
			cin >> temp;
			cout << endl;
			final.lookup(temp);
			cout << endl;
			break;
		case 4:
			// �������� BinaryFileManager
			cout << endl << "----------------- �������� BinaryFileManager -----------------" << endl << endl;
			for (int i = 1; i <= fileSize; i++) { // �������� ������
				file.lookup(i);
				cout << endl;
			}

			// �������� ������ ��� ��������
			patient.cardNumber = 67189;
			patient.diseaseCode = 12156;
			strcpy_s(patient.doctorLastName, "Testov Test Testovich");

			cout << "-----------------";
			cout << endl << "cardNumber: " << 67189 << endl
				<< "diseaseCode: " << 12156 << endl
				<< "doctorLastName: Testov Test Testovich" << endl;
			cout << "-----------------> ";
			file.insert(patient); // ������� ������ � ����
			cout << endl << "-----------------";
			cout << endl << "cardNumber: 78910" << endl;
			cout << "-----------------> ";
			file.remove(78910); // �������� ������ � �����
			fileSize++; // ���������� ������� �����
			cout << endl << "-> ����� ���������:" << endl << endl;
			for (int i = 1; i <= fileSize; i++) {
				file.lookup(i);
				cout << endl;
			}

			// �������� HashTable
			cout << "----------------- �������� HashTable -----------------" << endl;
			// �������� ������ ��� ��������
			hash.insert(63711, 1);
			hash.insert(69711, 2);
			hash.insert(78910, 3);
			hash.insert(79100, 4);
			hash.insert(65128, 5);

			cout << hash.lookup(63711) << endl;
			cout << hash.lookup(69711) << endl;
			cout << hash.lookup(78910) << endl;
			cout << hash.lookup(79100) << endl;
			cout << hash.lookup(65128) << endl;
			cout << endl << "-----------------" << endl;
			cout << "key: 60202, recordIndex: 6" << endl;
			cout << "-----------------> ";
			hash.insert(60202, 6); // ������� ������ � ���-�������
			cout << "������ ������� ���������." << endl;
			cout << endl << "-----------------" << endl;
			cout << "key: 65128" << endl;
			hash.remove(65128); // �������� ������ �� ���-�������
			cout << "-----------------> ";
			cout << "������ ������� �������." << endl;
			cout << endl << "-> ����� ���������:" << endl;
			cout << hash.lookup(63711) << endl; // �������� ������
			cout << hash.lookup(69711) << endl;
			cout << hash.lookup(78910) << endl;
			cout << hash.lookup(79100) << endl;
			cout << hash.lookup(65128) << endl;
			cout << hash.lookup(60202) << endl;
			cout << endl;
			break;
		default: 
			cout << endl << "������� ��� � ������!" << endl << endl;
			break;
		}
	}
}
