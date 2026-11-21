/* FazOS Kernel 0.1 - egitim amacli minimal cekirdek (VGA metin, PS/2 klavye, kabuk). ASCII metin kullanilir. */
typedef unsigned char u8; typedef unsigned short u16; typedef unsigned int u32;
static inline void outb(u16 p,u8 v){__asm__ volatile("outb %0,%1"::"a"(v),"Nd"(p));}
static inline u8 inb(u16 p){u8 v;__asm__ volatile("inb %1,%0":"=a"(v):"Nd"(p));return v;}
static inline void outw(u16 p,u16 v){__asm__ volatile("outw %0,%1"::"a"(v),"Nd"(p));}
static volatile u16 *const VGA=(volatile u16*)0xB8000;
static int cx,cy;static u8 col=0x0F;
static void cur(void){u16 p=cy*80+cx;outb(0x3D4,14);outb(0x3D5,p>>8);outb(0x3D4,15);outb(0x3D5,p&255);}
static void scroll(void){for(int i=0;i<80*24;i++)VGA[i]=VGA[i+80];for(int i=80*24;i<80*25;i++)VGA[i]=(col<<8)|' ';cy=24;}
static void putc(char c){
  if(c=='\n'){cx=0;cy++;}
  else if(c=='\b'){if(cx>0){cx--;VGA[cy*80+cx]=(col<<8)|' ';}}
  else{VGA[cy*80+cx]=(col<<8)|(u8)c;if(++cx==80){cx=0;cy++;}}
  if(cy>=25)scroll();cur();}
static void puts(const char*s){while(*s)putc(*s++);}
static void cls(void){for(int i=0;i<80*25;i++)VGA[i]=(col<<8)|' ';cx=cy=0;cur();}
static void putu(u32 n){char b[12];int i=11;b[i]=0;do{b[--i]='0'+n%10;n/=10;}while(n);puts(b+i);}
static void put2(u32 n){putc('0'+n/10);putc('0'+n%10);}
static const char map[128]={0,27,'1','2','3','4','5','6','7','8','9','0','-','=','\b','\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c','v','b','n','m',',','.','/',0,'*',0,' '};
static const char smap[128]={0,27,'!','@','#','$','%','^','&','*','(',')','_','+','\b','\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',0,'A','S','D','F','G','H','J','K','L',':','"','~',0,'|','Z','X','C','V','B','N','M','<','>','?',0,'*',0,' '};
static int shift;
static char getch(void){
  for(;;){
    u8 st;do{st=inb(0x64);__asm__ volatile("pause");}while(!(st&1));
    u8 s=inb(0x60);if(st&0x20)continue;           /* fare verisini atla */
    if(s==0x2A||s==0x36){shift=1;continue;}
    if(s==0xAA||s==0xB6){shift=0;continue;}
    if(s&0x80)continue;
    char c=shift?smap[s]:map[s];if(c)return c;}}
static int eq(const char*a,const char*b){while(*a&&*a==*b){a++;b++;}return *a==*b;}
static int pre(const char*s,const char*p){while(*p){if(*s++!=*p++)return 0;}return 1;}
static u8 cmos(u8 r){outb(0x70,r);return inb(0x71);}
static u32 memkb;
static void delay(u32 n){for(volatile u32 i=0;i<n;i++);}
static void halt(void){for(;;)__asm__ volatile("cli;hlt");}
static void splash(void){
  cls();col=0x0F;cy=8;cx=34;puts("F a z O S");cy=10;cx=30;puts("FEAS Software 1980-2027");
  cy=13;cx=28;puts("[");cx=51;puts("]");
  for(int i=0;i<22;i++){cy=13;cx=29+i;col=0x70;putc(' ');col=0x0F;delay(4000000);}
  col=0x0F;cls();}
static void cmd(char*l){
  if(eq(l,"yardim")){puts("yardim surum temizle saat hafiza bite87 yaz <metin> kapat yeniden\n");}
  else if(eq(l,"surum")){puts("FazOS Kernel 0.1 (egitim amacli, x86 32 bit, Multiboot)\nGercek masaustu icin Linux tabanli FazOS 2.0 projesine bakin.\n");}
  else if(eq(l,"temizle"))cls();
  else if(eq(l,"saat")){u8 b=cmos(0x0B);u8 h=cmos(4),m=cmos(2),s=cmos(0);
    if(!(b&4)){h=(h&15)+(h>>4)*10;m=(m&15)+(m>>4)*10;s=(s&15)+(s>>4)*10;}
    put2(h);putc(':');put2(m);putc(':');put2(s);puts(" (UTC, RTC)\n");}
  else if(eq(l,"hafiza")){puts("Bellek: ");putu(memkb/1024);puts(" MB (ust bellek)\n");}
  else if(eq(l,"bite87")){puts("ISIRIK OLAYI - 1987\nBir animatronik, bir kisinin on lobunu isirdi; kurban hayatta kaldi.\nSorumlu konusunda hayran teorileri farklidir (Mangle teorisi yaygin).\n");}
  else if(pre(l,"yaz ")){puts(l+4);putc('\n');}
  else if(eq(l,"kapat")){puts("Kapatiliyor...\n");outw(0x604,0x2000);outw(0xB004,0x2000);outw(0x4004,0x3400);puts("Otomatik kapanmadi; gucu elle kesebilirsiniz.\n");halt();}
  else if(eq(l,"yeniden")){while(inb(0x64)&2);outb(0x64,0xFE);halt();}
  else if(l[0])puts("Bilinmeyen komut. 'yardim' yazin.\n");}
void kmain(u32 magic,u32*mb){
  if(magic==0x2BADB002&&(mb[0]&1))memkb=mb[2];
  splash();
  puts("FazOS Kernel 0.1\nMultiboot: ");puts(magic==0x2BADB002?"tamam":"HATA");puts("\n'yardim' yazin.\n\n");
  char buf[80];
  for(;;){
    puts("fazos> ");int n=0;
    for(;;){char c=getch();
      if(c=='\n'){putc('\n');break;}
      if(c=='\b'){if(n>0){n--;putc('\b');}continue;}
      if(n<79&&c>=' '){buf[n++]=c;putc(c);}}
    buf[n]=0;cmd(buf);}}
