#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//const char *seq1="AGGCTTTG";
//const char *seq2="CATGTAT";

const char *seq1="ACTGATTCA";
const int n = 9;
const char *seq2="ACGCATCA";
const int m = 8;

unsigned char scores[n+1][m+1];
char moves[n+1][m+1];

char aug1[n+m+2];
char aug2[n+m+2];

void initial_conditions() {
  for (int i=0; i<=n; i++) {
    for (int j=0; j<=m; j++) {
      scores[i][j] = 0;
      moves[i][j] = '.';
    }
  }

  for (unsigned char i=1; i<=n; i++) {
    scores[i][0] = i;
    moves[i][0] = 'I';
  }

  for (unsigned char j=1; j<=m; j++) {
    scores[0][j] = j;
    moves[0][j] = 'D';
  }

  for (int i=0; i<=n+m+1; i++) {
    aug1[i] = '\0';
    aug2[i] = '\0';
  }
}

void print_scores(int row) {
  for (int i=0; i<=row; i++) {
    for (int j=0; j<=m; j++) {
      Serial.print(scores[i][j]);
      Serial.print(" ");
    }
    Serial.println("");
  }
  Serial.println("");
}

unsigned char partial_edit_alignment(int row) {
  for (unsigned char j=1; j<=m; j++) {
    unsigned char match = scores[row-1][j-1];
    if (seq1[row-1] != seq2[j-1])
      match += 1;
    unsigned char ins = scores[row-1][j] + 1;
    unsigned char del = scores[row][j-1] + 1;

    scores[row][j] = del;
    moves[row][j] = 'D';
    if (ins < scores[row][j]) {
      scores[row][j] = ins;
      moves[row][j] = 'I';
    }
    if (match < scores[row][j]) {
      scores[row][j] = match;
      moves[row][j] = 'M';
    }
  }
  return scores[row][m];
}

void backtrack(int row, char **a1, char **a2) {
  int i = row;
  int j = m;
  int idx = n+m;
  aug1[idx+1] = '\0';
  aug2[idx+1] = '\0';

  while ((i>0) || (j>0)) {
    if (moves[i][j] == 'M') {
      aug1[idx] = seq1[i-1];
      aug2[idx] = seq2[j-1];
      idx--;
      i--;
      j--;
    } else {
      if (moves[i][j] == 'D') {
        aug2[idx] = seq2[j-1];
        aug1[idx] = '-';
        idx--;
        j--;
      } else {
        aug1[idx] = seq1[i-1];
        aug2[idx] = '-';
        idx--;
        i--;
      }
    }
  }
  *a1 = &(aug1[idx+1]);
  *a2 = &(aug2[idx+1]);
}

void setup() {
  initial_conditions();
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.print("Aligning:");
  lcd.setCursor(0, 1);
  lcd.print(seq1);
  lcd.print(" and ");
  lcd.print(seq2);
  delay(1500);

  const int nshift = 5 + n + m - 16;
  for (int i=0; i<nshift; i++) {
    lcd.scrollDisplayLeft();
    delay(500);
  }
  delay(500);
}

void loop() {
  int row;
  char *a1;
  char *a2;
  for (int row=1; row <= n; row++) {
    unsigned char d = partial_edit_alignment(row);
    backtrack(row, &a1, &a2);
    Serial.println("");
    Serial.println(d);
    Serial.println(a2);
    Serial.println(a1);
    
    lcd.clear();
    lcd.print(a2);
    lcd.setCursor(0, 1);
    lcd.print(a1);

    delay(750);
  }
  delay(500);
  for (int i=0; i<5; i++) {
    lcd.clear();
    delay(250);
    lcd.print(a2);
    lcd.setCursor(0, 1);
    lcd.print(a1);
    delay(250);
  }
  delay(5000);
}


