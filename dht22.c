#include <wiringPi.h>
#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <malloc.h>

#define MAXTIMINGS	85
#define REFRESHTIME 100 //센서 탐지 대기 시간. 밀리초 기준.
#define DBUPDATETIME 10// 데이터베이스를 새로 연결하기 위한 시간. 분단위 기준.

MYSQL _mysql; //DB 연결기

char *db_server = "203.250.32.155";
char *db_user = "snslab";
char *db_passwd = "snslab";
char *db_database = "snslab";
char *db_table = "SmartPlanting";

//함수에서 쓰이는 변수, 삭제 금지!
float ih, ic = 0.0;
static int checkNum[10] = { 0, }; // check how many get data
int dht22_dat[5] = { 0, };
int _concatStrings_actualTotal = 0; // 문자열 합치기용 임시
char *_concateStrings_strconcat; // 문자열 합치기용
char *_query; // 데이터베이스 쿼리문을 저장
float _read_dht22_dat_sensorData[2] = { 0.0f, 0.0f }; // 온습도 값 저장
int valuesOnTerminal = 1; // 화면에 값이 보이도록 지정. 0 = 안보임, 1 = 보임.
int uploadTrue = 1; //서버로 업로드 여부를 결정. 0 = 업로드 안함, 1 = 업로드 함.
static float sensor_data[20] = { 0.0, }; //센서 데이터 동글
static unsigned int arduino_data[8] = { 0, }; //아두이노 데이터 저장 배열 들어오는 값을 차례대로 int 형으로 저장하면된다.
static unsigned short flowmeter_data[2] = { 0, }; //유량계 데이터.

int mysql_exec_sql(MYSQL *mysql, const char *create_definition) {
	return mysql_real_query(mysql, create_definition, strlen(create_definition));
}
void mysql_initial() {
	printf("데이터베이스 초기화\n");
	if (mysql_init(&_mysql) == NULL) {
		printf("데이터베이스 헤더 호출 실패 \n");
		exit(1);
	}

	//Conect to mysql server
	if (!mysql_real_connect(&_mysql, db_server, db_user, db_passwd, NULL, 0,
			NULL, 0)) {
		printf("데이터베이스 연결 실패: %s\n", mysql_error(&_mysql));
		exit(1);
	}

	//Select Database
	if (mysql_select_db(&_mysql, db_database) != 0) {
		printf("%s 데이터베이스 사용 실패: %s\n", db_database, mysql_error(&_mysql));
		exit(1);
	}
}


void mysql_sendData(unsigned int first_humidity, int first_temperature,
		unsigned int second_humidity, int second_temperature,
		unsigned int third_humidity, int third_temperature,
		unsigned int b1_a0, unsigned int b1_a1, unsigned int b1_a2,
		unsigned int b1_a3, unsigned int b2_a0, unsigned int b2_a1,
		unsigned int b2_a2, unsigned int b2_a3, unsigned short first_flowMeter,
		unsigned short second_flowMeter) {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char timeVal[50] = { '\0', }; //Current date&time in second
	char first_humVal[50] = { '\0', }; //Humidity value in first DHT22 sensor
	char first_tmpVal[50] = { '\0', }; //Temperature value in first DHT22 sensor
	char second_humVal[50] = { '\0', }; //Humidity value in second DHT22 sensor
	char second_tmpVal[50] = { '\0', }; //Temperature value in second DHT22 sensor
	char third_humVal[50] = { '\0', }; //Humidity value in third DHT22 sensor
	char third_tmpVal[50] = { '\0', }; //Temperature value in third DHT22 sensor
	char b1_a0_Val[50] = { '\0', }; //First analog value(A0) in board 1(ACM0)
	char b1_a1_Val[50] = { '\0', }; //Second analog value(A1) in board 1(ACM0)
	char b1_a2_Val[50] = { '\0', }; //Third analog value(A2) in board 1(ACM0)
	char b1_a3_Val[50] = { '\0', }; //Fourth analog value(A3) in board 1(ACM0)
	char b2_a0_Val[50] = { '\0', }; //First analog value(A0) in board 2(ACM1)
	char b2_a1_Val[50] = { '\0', }; //Second analog value(A1) in board 2(ACM1)
	char b2_a2_Val[50] = { '\0', }; //Third analog value(A2) in board 2(ACM1)
	char b2_a3_Val[50] = { '\0', }; //Fourth analog value(A3) in board 2(ACM1)
	char first_flowMeterVal[50] = { '\0', }; //First flowmeter value;
	char second_flowMeterVal[50] = { '\0', }; //Second flowmeter value;

	sprintf(timeVal, "'%4d-%02d-%02d %02d:%02d:%02d'", tm.tm_year + 1900,
			tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	if (uploadTrue == 1 && valuesOnTerminal == 1) {
		printf("  │ %4d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900,
				tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	}

	//Convert values in to char array
	sprintf(first_humVal, "%d", first_humidity);
	sprintf(first_tmpVal, "%d", first_temperature);
	sprintf(second_humVal, "%d", second_humidity);
	sprintf(second_tmpVal, "%d", second_temperature);
	sprintf(third_humVal, "%d", third_humidity);
	sprintf(third_tmpVal, "%d", third_temperature);
	sprintf(b1_a0_Val, "%d", b1_a0);
	sprintf(b1_a1_Val, "%d", b1_a1);
	sprintf(b1_a2_Val, "%d", b1_a2);
	sprintf(b1_a3_Val, "%d", b1_a3);
	sprintf(b2_a0_Val, "%d", b2_a0);
	sprintf(b2_a1_Val, "%d", b2_a1);
	sprintf(b2_a2_Val, "%d", b2_a2);
	sprintf(b2_a3_Val, "%d", b2_a3);
	sprintf(first_flowMeterVal, "%hu", first_flowMeter);
	sprintf(second_flowMeterVal, "%hu", second_flowMeter);

	//쿼리문 생성.
	_query = (char *) calloc(1000, sizeof(char));
	_query = strcat(_query, "INSERT INTO ");
	_query = strcat(_query, db_table);
	_query = strcat(_query, " VALUE('',");
	_query = strcat(_query, timeVal);
	_query = strcat(_query, ",");
	_query = strcat(_query, first_humVal);
	_query = strcat(_query, ",");
	_query = strcat(_query, first_tmpVal);
	_query = strcat(_query, ",");
	_query = strcat(_query, second_humVal);
	_query = strcat(_query, ",");
	_query = strcat(_query, second_tmpVal);
	_query = strcat(_query, ",");
	_query = strcat(_query, third_humVal);
	_query = strcat(_query, ",");
	_query = strcat(_query, third_tmpVal);
	_query = strcat(_query, ",");
	_query = strcat(_query, b1_a0_Val);
	_query = strcat(_query, ",");
	_query = strcat(_query, b1_a1_Val);
	_query = strcat(_query, ",");
	_query = strcat(_query, b1_a2_Val);
	_query = strcat(_query, ",");
	_query = strcat(_query, b1_a3_Val);
	_query = strcat(_query, ",");
	_query = strcat(_query, b2_a0_Val);
	_query = strcat(_query, ",");
	_query = strcat(_query, b2_a1_Val);
	_query = strcat(_query, ",");
	_query = strcat(_query, b2_a2_Val);
	_query = strcat(_query, ",");
	_query = strcat(_query, b2_a3_Val);
	_query = strcat(_query, ",");
	_query = strcat(_query, first_flowMeterVal);
	_query = strcat(_query, ",");
	_query = strcat(_query, second_flowMeterVal);
	_query = strcat(_query, ");");
	_concatStrings_actualTotal = 0;
	if (mysql_exec_sql(&_mysql, _query) != 0) {
		printf("query : %s\n", _query);
		printf("데이터 삽입 실패: %s\n", mysql_error(&_mysql));
	}

	free(_query);
	_query = NULL;
	//	memset(_query, ' ', 101); // 100개로 지정하면 끝에 i가 발생하여 쿼리를 날리지 못함.

}
void mysql_end() {
	mysql_close(&_mysql);
	printf("데이터베이스 연결 종료\n");
}

/*
 * 아두이노로부터 아날로그 데이터를 읽어옴
 */
void read_analogData(int deviceNumber) {

}

void read_dht22_data(int DHT22_PIN, int check) {
	uint8_t laststate = HIGH;
	uint8_t counter = 0; //don't know
	uint8_t j = 0, i;

	dht22_dat[0] = dht22_dat[1] = dht22_dat[2] = dht22_dat[3] = dht22_dat[4] =
			0;

	/* pull pin down for 18 milliseconds */
	pinMode(DHT22_PIN, OUTPUT);
	digitalWrite(DHT22_PIN, LOW);
	delay(18); // maybe get data pin

	digitalWrite(DHT22_PIN, HIGH);
	delayMicroseconds(30);
	/* prepare to read the pin */
	pinMode(DHT22_PIN, INPUT);

	/* detect change and read data */
	for (i = 0; i < MAXTIMINGS; i++) {
		counter = 0;
		while (digitalRead(DHT22_PIN) == laststate) {
			counter++;
			delayMicroseconds(1); //reading delay
			if (counter == 255) {
				break;
			}
		}
		laststate = digitalRead(DHT22_PIN);

		if (counter == 255)
			break;

		/* ignore first 3 transitions */
		if ((i >= 4) && (i % 2 == 0)) {
			/* shove each bit into the storage bytes */
			dht22_dat[j / 8] <<= 1;
			if (counter > 16)
				dht22_dat[j / 8] |= 1;
			j++;
		}
	}

	/*
	 * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
	 * print it out if data is good
	 */
	if ((j >= 40)
			&& (dht22_dat[4]
					== ((dht22_dat[0] + dht22_dat[1] + dht22_dat[2]
							+ dht22_dat[3]) & 0xFF))) {
		float h = (float) ((dht22_dat[0] << 8) + dht22_dat[1]) / 10;
		if (h > 100) {
			h = dht22_dat[0];    // for DHT11
		}
		float c = (float) (((dht22_dat[2] & 0x7F) << 8) + dht22_dat[3]) / 10;
		if (c > 125) {
			c = dht22_dat[2];    // for DHT11
		}
		if (dht22_dat[2] & 0x80) {
			c = -c;
		}
		float f = c * 1.8f + 32;
		//mysql_sendDHT22Data(DHT22_PIN, h * 10, c * 10);
		sensor_data[check * 2] += h;
		sensor_data[(check * 2) - 1] += c;
		checkNum[check - 1] += 1;
		//printf( "Humidity = %.1f %% Temperature = %.1f *C (%.1f *F)\n", h, c, f );//.99999 get solution of problem
	} else {
		//printf( "Data not good, skip\n" );
		/*mysql_exec_sql(&_mysql,
		 "INSERT INTO DHT22 VALUE('','1990-10-24 00:00:00',0.0,0.0);");
		 */
	}
}

void sig_handler(int signo) {
	printf("\n작업 종료됨\n");
	printf("SIG :%d\n", signo);
	exit(0);
}
int main(int argc, char *argv[]) {
	int DHT22PinNumbers[10] = { 0, };
	int DHT22PinNumbersCounter = 0;
	int databaseRefreshCount = DBUPDATETIME;
	int recordingTime = REFRESHTIME;

	int check;
	//wiringPi 점검.
	if (wiringPiSetup() == -1) {
		printf("wiringPi로딩에 실패하였습니다 \n");
		exit(1);
	}
	// Arguments 갯수 확인.
	if (argc < 2) {
		printf("장지와 연결된 GPIO 핀 번호가 필요합니다.\n");
		printf("사용법 : sudo ./dht22 핀 번호_1, 핀 번호_2, ...\n");
		exit(1);
	} else if (argc > 10) {
		printf("이 프로그램에서 너무 많은 GPIO핀의 동시 사용은 프로그램의 성능저하를 불러일으킬 수 있습니다.\n");
	} else {
		for (int i = 1; i < argc; i++) {
			//check = _stringToInteger(argv[i]);
			check = atoi(argv[i]);
			printf("check : %d\n", check);
			//부가 옵션
			/*if (check > 31) {
			 //서버로 업로드
			 if (!strcmp(argv[i], "upload")) {
			 uploadTrue = 1;
			 continue;
			 } else if (!strcmp(argv[i], "display")) {
			 valuesOnTerminal = 1;
			 continue;
			 } else {
			 printf("unknown Command : %s", argv[i]);
			 continue;
			 }
			 } else if (!(check < 32)) {
			 printf("핀 번호가 허용 범위를 벗어났습니다.\n");
			 exit(1);
			 }
			 */
			DHT22PinNumbers[DHT22PinNumbersCounter] = check;
			DHT22PinNumbersCounter += 1;
		}
	}

	//SIG 핸들러
	//signal(SIGINT, (void *) sig_handler);

	//데이터베이스 재연결을 위한 데이터 최대 수집 횟수 계산.
	databaseRefreshCount = (int) (databaseRefreshCount * 60 * (1000 / recordingTime));

	//DHT22센서 데이터 수집
	printf("valuesOnTerminal : %d\n", valuesOnTerminal);
	int count;
	printf(" start while\n");
	while (1) {
		count = 0;
		if (uploadTrue) {
			printf(" init mysql\n");
			mysql_initial();
		}
		while (count < databaseRefreshCount) {
			printf("──┬─────────────────────────────\n");
			if (DHT22PinNumbersCounter > 1) {
				for (int x = 0; x < 10; x++) {
					for (int i = 1; i < DHT22PinNumbersCounter + 1; i++) {
						read_dht22_data(DHT22PinNumbers[i - 1], i);
						delay(250);
					}
					delay(REFRESHTIME);
				}
				for (int i = 1; i < DHT22PinNumbersCounter + 1; i++) {
					if (checkNum[i - 1] == 0) {
						printf("Sensor Return zero data\n");
						sensor_data[(i * 2)] = sensor_data[(i * 2) - 1] = 0.0;
					} else {
						printf("Humidity = %.1f %% Temperature = %.1f *C \n",
								sensor_data[(i * 2)] / checkNum[i - 1],
								sensor_data[((i * 2) - 1)] / checkNum[i - 1]);
					}
				}
				printf("send query to SmartPlanting");
				//쿼리 전송
				mysql_sendData(
						(unsigned int) (sensor_data[2] / checkNum[0] * 10),
						(int) (sensor_data[1] / checkNum[0] * 10),
						(unsigned int) (sensor_data[4] / checkNum[1] * 10),
						(int) (sensor_data[3] / checkNum[1] * 10),
						(unsigned int) (sensor_data[6] / checkNum[2] * 10),
						(int) (sensor_data[5] / checkNum[2] * 10),
						arduino_data[0], arduino_data[1], arduino_data[2],
						arduino_data[3], arduino_data[4], arduino_data[5],
						arduino_data[6], arduino_data[7], flowmeter_data[0],
						flowmeter_data[1]);
				//쿼리 날리는 메소드 센서 데이터 배열과 아두이노에서 보내온 데이터를 저장한 배열을
				// 보낸다. 특별한 메소드를 구현하지 않았기 때문에 원하는 배열을 직접 넣어야된다.
				//데이터를 보내기 전에 아두이노 데이터 배열에 데이터를 삽입하면 된다.

				for (int y = 0; y < 20; y++) {
					sensor_data[y] = 0.0;
				}
				for (int y = 0; y < 10; y++) {
					checkNum[y] = 0;
				}
				for (int y = 0; y < 8; y++) {
					arduino_data[y] = 0;
				}
			} else if (DHT22PinNumbersCounter == 1) {
				for (int x = 0; x < 10; x++) {
					read_dht22_data(DHT22PinNumbers[0], 1);
					delay(REFRESHTIME);
				}
				printf("Humidity = %.1f %% Temperature = %.1f *C \n",
						sensor_data[2] / checkNum[0],
						sensor_data[1] / checkNum[0]);
				if (checkNum[0] == 0) {
					printf("Zero data was send\n");
				}

			} else
				printf("pin num is under 1. please get correct pin number\n");

			printf("──┴─────────────────────────────\n");
			for (int y = 0; y < 20; y++) {
				sensor_data[y] = 0.0;
			}
			for (int y = 0; y < 10; y++) {
				checkNum[y] = 0;
			}
			count++;

		}

		if (uploadTrue) {

			mysql_end();
		}
	}

	return (0);
}
