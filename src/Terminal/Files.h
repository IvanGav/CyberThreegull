#pragma once
#include "Terminal.h"

/* Making Puzzles files*/

// Depression: Adjust Happy Value

// Depression: Adjust Happy Value
void depression(File& io) {
    io_print(io, "<!DOCTYPE html>"a);
    io_print(io, "<html lang=\"en\">"a);
    io_print(io, "<head>"a);
    io_print(io, "  <meta charset=\"UTF-8\">"a);
    io_print(io, "  <title>Depression Puzzle</title>"a);
    io_print(io, "</head>"a);
    io_print(io, "<body>"a);
    io_print(io, "  <h1>Adjust the Happy Value</h1>"a);
    io_print(io, "  <p>Current mood level:</p>"a);
    io_print(io, "  <!-- Change this value to a positive number to “cure” the depression -->"a);
    io_print(io, "  <input type=\"number\" id=\"happyValue\" value=\"-5\">"a);
    io_print(io, "  <p>Click Save after you make it positive!</p>"a);
    io_print(io, "</body>"a);
    io_print(io, "</html>"a);
}

void depresssion_facts(File& io) {
	io_print(io, "Depression is a common mental health disorder that affects millions of people worldwide."a);
	io_print(io, "It can cause persistent feelings of sadness, hopelessness, and a lack of interest in activities."a);
	io_print(io, "Symptoms may include changes in appetite, sleep disturbances, fatigue, and difficulty concentrating."a);
	io_print(io, "Treatment options include therapy, medication, lifestyle changes, and support from loved ones."a);
	io_print(io, "It's important to seek help if you or someone you know is struggling with depression."a);
}

// ADHD: ADD Firewall to Block distractions, need to block ips:
// 192.168.0.101, 192.168.0.102, 192.168.0.103, 192.168.0.104
void adhd(File& io) {
    io_print(io, "# Nmap 7.93 scan report for ADHD Puzzle"a);

    io_print(io, ""a);
    io_print(io, "Nmap scan report for 192.168.0.101 (tiktok.com)"a);
    io_print(io, "Host is up (0.00098s latency)."a);
    io_print(io, "Not shown: 996 closed ports"a);
    io_print(io, "PORT     STATE SERVICE     VERSION"a);
    io_print(io, "1935/tcp open  rtmp        Adobe Flash RTMP"a);
    io_print(io, "  | traffic: 500MB video stream"a);
    io_print(io, "80/tcp   open  http        Apache httpd"a);
    io_print(io, "  | traffic:  50MB video chunks"a);
    io_print(io, "443/tcp  open  ssl/http    nginx"a);
    io_print(io, "  | traffic:  20MB API calls"a);

    io_print(io, ""a);
    io_print(io, "Nmap scan report for 192.168.0.102 (youtube.com)"a);
    io_print(io, "Host is up (0.00120s latency)."a);
    io_print(io, "Not shown: 997 closed ports"a);
    io_print(io, "PORT     STATE SERVICE"a);
    io_print(io, "80/tcp   open  http"a);
    io_print(io, "  | traffic: 1.2GB live stream"a);
    io_print(io, "443/tcp  open  ssl/http"a);
    io_print(io, "  | traffic:  100MB comment API"a);

    io_print(io, ""a);
    io_print(io, "Nmap scan report for 192.168.0.103 (reddit.com)"a);
    io_print(io, "Host is up (0.00045s latency)."a);
    io_print(io, "Not shown: 998 closed ports"a);
    io_print(io, "PORT     STATE SERVICE"a);
    io_print(io, "80/tcp   open  http"a);
    io_print(io, "  | traffic:  200MB image loads"a);
    io_print(io, "443/tcp  open  ssl/http"a);
    io_print(io, "  | traffic:   30MB comment refresh"a);

    io_print(io, ""a);
    io_print(io, "Nmap scan report for 192.168.0.104 (instagram.com)"a);
    io_print(io, "Host is up (0.00105s latency)."a);
    io_print(io, "Not shown: 997 closed ports"a);
    io_print(io, "PORT     STATE SERVICE"a);
    io_print(io, "80/tcp   open  http"a);
    io_print(io, "  | traffic:  300MB photo feed"a);
    io_print(io, "443/tcp  open  ssl/http"a);
    io_print(io, "  | traffic:   75MB story loads"a);

    io_print(io, ""a);
    io_print(io, "# End of ADHD distractions scan"a);
}

void adhd_facts(File& io) {
	io_print(io, "ADHD (Attention Deficit Hyperactivity Disorder) is a neurodevelopmental disorder."a);
	io_print(io, "It affects both children and adults, causing inattention, hyperactivity, and impulsivity."a);
	io_print(io, "Symptoms can vary widely among individuals and may change over time."a);
	io_print(io, "ADHD is often treated with a combination of behavioral therapy and medication."a);
	io_print(io, "Early diagnosis and intervention can significantly improve outcomes for individuals with ADHD."a);
}


// Anxiety: Replace negative prompts with positive ones
void anxiety(File& io) {
    // List of negative self‑talk prompts for the user to flip positive
    io_print(io, "I always mess up"a);
    io_print(io, "Nothing ever goes right for me"a);
    io_print(io, "I'm a failure"a);
    io_print(io, "I can't do anything right"a);
    io_print(io, "No one cares about me"a);
    io_print(io, "I'll never get better"a);
    io_print(io, "Why bother trying at all?"a);
    io_print(io, "Everything is my fault"a);
}

void anxiety_facts(File& io) {
    io_print(io, "Anxiety is a normal and often healthy emotion."a);
    io_print(io, "It becomes a problem when it interferes with daily life."a);
    io_print(io, "Common anxiety disorders include generalized anxiety disorder (GAD), panic disorder, and social anxiety disorder."a);
    io_print(io, "Symptoms may include excessive worry, restlessness, fatigue, and difficulty concentrating."a);
    io_print(io, "Treatment options include therapy, medication, and lifestyle changes."a);
}


// Brainrot: Remove the latest digital brainrots from memory
// Brainrot: Remove TikTok-related memes and brainrots from memory
void brainrot(File& io) {
    io_print(io, "Skibidi"a);
    io_print(io, "Skibidi toilet"a);
    io_print(io, "Gyatt"a);
    io_print(io, "Mewing"a);
    io_print(io, "Mew"a);
    io_print(io, "Rizz"a);
    io_print(io, "Rizzing"a);
    io_print(io, "Rizzler"a);
    io_print(io, "on Skibidi"a);
    io_print(io, "Sigma"a);
    io_print(io, "What the sigma"a);
    io_print(io, "Ohio"a);
    io_print(io, "Bussin’"a);
    io_print(io, "Cook"a);
    io_print(io, "Cooking"a);
    io_print(io, "Let him cook"a);
    io_print(io, "Baddie"a);
    io_print(io, "Skibidi rizz"a);
    io_print(io, "Fanum tax"a);
    io_print(io, "Fanum taxing"a);
    io_print(io, "Drake"a);
    io_print(io, "Nonchalant dread head"a);
    io_print(io, "Aura"a);
    io_print(io, "Grimace shake"a);
    io_print(io, "Edging"a);
    io_print(io, "Edge"a);
    io_print(io, "Goon"a);
    io_print(io, "Gooning"a);
    io_print(io, "Looks maxing"a);
    io_print(io, "Alpha"a);
    io_print(io, "Griddy"a);
    io_print(io, "Chris Tyson"a);
    io_print(io, "Diddy"a);
    io_print(io, "Imagine if ninja got a low taper fade"a);
    io_print(io, "Put the fries in the bag"a);
    io_print(io, "What’s up brother"a);
    io_print(io, "Baby gronk"a);
    io_print(io, "Livvy Dunne"a);
    io_print(io, "On skib"a);
    io_print(io, "Duke Dennis"a);
    io_print(io, "I see trees of Breen"a);
    io_print(io, "Oh when the saints go marching in"a);
    io_print(io, "Blud"a);
    io_print(io, "Lil bro"a);
    io_print(io, "Bite the curb"a);
    io_print(io, "Bazinga"a);
    io_print(io, "Brisket song"a);
    io_print(io, "English or Spanish"a);
    io_print(io, "5 nights at Diddys"a);
    io_print(io, "Devious lick"a);
    io_print(io, "Fanta in my system song"a);
    io_print(io, "Quandale Dingle"a);
    io_print(io, "Sus"a);
    io_print(io, "Sussy"a);
    io_print(io, "Imposter"a);
    io_print(io, "Among us"a);
    io_print(io, "TikTok rizz party"a);
    io_print(io, "Oi oi oi"a);
    io_print(io, "Reese’s pieces and lifesaver gummies meme"a);
    io_print(io, "Devious"a);
    io_print(io, "Ei ei ei"a);
    io_print(io, "Ratio"a);
    io_print(io, "L"a);
    io_print(io, "L bozo"a);
    io_print(io, "L + ratio"a);
    io_print(io, "Brain rot"a);
    io_print(io, "IShowSpeed / IShowMeat"a);
    io_print(io, "Bing chilling"a);
    io_print(io, "Bomboclat"a);
    io_print(io, "What da dog doin"a);
    io_print(io, "Mog"a);
    io_print(io, "Mogging"a);
    io_print(io, "Yap"a);
    io_print(io, "Yapping"a);
    io_print(io, "Yapper"a);
    io_print(io, "Sticking out your gyatt for the rizzler"a);
    io_print(io, "Caseoh"a);
    io_print(io, "Goonmaxing"a);
    io_print(io, "Freddy fazbear"a);
    io_print(io, "Kai cenat"a);
    io_print(io, "Harlem shake"a);
    io_print(io, "Glizzy"a);
    io_print(io, "Smurf cat"a);
    io_print(io, "Kai cenat rizz"a);
    io_print(io, "Digital circus"a);
    io_print(io, "1 2 buckle my shoe"a);
    io_print(io, "Oil up"a);
    io_print(io, "Did you pray today"a);
    io_print(io, "Cap / stop the cap"a);
    io_print(io, "Only in Ohio"a);
}

void brainrot_facts(File& io) {
	io_print(io, "Brain rot refers to the mental fatigue and cognitive overload caused by excessive digital consumption."a);
	io_print(io, "It can lead to decreased attention span, memory issues, and difficulty focusing."a);
	io_print(io, "Symptoms may include irritability, anxiety, and a sense of disconnection from reality."a);
	io_print(io, "Taking regular breaks from screens and engaging in offline activities can help alleviate brain rot."a);
	io_print(io, "Practicing mindfulness and limiting digital consumption can improve mental clarity."a);
}


// Incel: Remove 4Chan from memory
void incel(File& io) {
    io_print(io, "Removing 4Chan from memory..."a);
    // Extended incel greentext threads and negative attitudes towards women
    io_print(io, "> be me, 23-year-old incel, living in mom's basement"a);
    io_print(io, "> never get a GF because all women only want a Chad"a);
    io_print(io, "> deodorant_stains.jpg"a);
    io_print(io, "> she friendzones me for some tall alpha male"a);
    io_print(io, "> they all have the same personality: narcissistic"a);
    io_print(io, "> women only care about status and looks"a);
    io_print(io, "> normie co-workers talk about their girlfriends nonstop"a);
    io_print(io, "> dating apps are a rigged system against nice guys"a);
    io_print(io, "> get ignored by every girl I message"a);
    io_print(io, "> average height guy lands a 10/10 girlfriend"a);
    io_print(io, "> Cross-reference personality: zero empathy"a);
    io_print(io, "> TRP said women are hypergamous"a);
    io_print(io, "> absolute units with Chad energy everywhere"a);
    io_print(io, "> Can’t even get a hello back"a);
    io_print(io, "> Day 374: still alone, rotting inside"a);
    io_print(io, "> MFW Stacy ignores my DMs"a);
    io_print(io, "> Why do women hate nice guys? Asking for a friend"a);
    io_print(io, "> Inceldom: the only way to learn self-hate"a);
    io_print(io, "> femoids are genetically engineered to destroy men"a);
    io_print(io, "> Overheard her laughing with Chad in cafeteria"a);
    io_print(io, "> Classroom full of her and her friends giggling"a);
    io_print(io, "> Tried talking, got ghosted instantly"a);
    io_print(io, "> She said she ‘just wants to be friends’ again"a);
    io_print(io, "> That friendzone dagger cut deeper than any blade"a);
    io_print(io, "> Policy says no harassment, but they harass nice guys"a);
    io_print(io, "> My mirror tells me I'm worthless every morning"a);
    io_print(io, "> She chose the jock, not the sensitive poet"a);
    io_print(io, "> All women are the same, deserve each other"a);
    io_print(io, "> Reddit says life is unfair, but they left out the part about women"a);
    io_print(io, "> The rope looks comfy, but mom said no"a);
    io_print(io, "> Mom told me to ‘smile more’, as if that fixes loneliness"a);
    io_print(io, "> Even my dog has friends, irony.jpg"a);
    io_print(io, "> She posted a selfie, likes skyrocketed in minutes"a);
    io_print(io, "> Meanwhile, my post gets zero responses"a);
    io_print(io, "> Learned about gaslighting, realized it’s my life story"a);
    io_print(io, "> They say ‘build confidence’, but I’m already done building"a);
    io_print(io, "> Women want a man who knows his worth, but won’t give me the chance"a);
    io_print(io, "> Lifetime achievement unlocked: perpetual rejection"a);
    io_print(io, "> Anon’s thread thinks incels are just trolls"a);
    io_print(io, "> If only I could troll women into noticing me"a);
    io_print(io, "> Life hack: don’t exist"a);
}

// Incel Facts: Provide factual information about the incel phenomenon
void incel_facts(File& io) {
    io_print(io, "Incel stands for 'involuntary celibate': individuals who struggle to form romantic or sexual relationships"a);
    io_print(io, "The term originated in the late 1990s as part of an online support group for people with social anxieties"a);
    io_print(io, "Modern incel communities are primarily found on imageboards and forums like 4chan and Reddit"a);
    io_print(io, "Inceldom is not recognized as a clinical diagnosis in the DSM-5 or ICD-10"a);
    io_print(io, "Many individuals identifying as incels report co-occurring mental health issues: depression, anxiety, and social isolation"a);
    io_print(io, "Incel ideology often includes beliefs of female hypergamy and male entitlement, leading to misogynistic attitudes"a);
    io_print(io, "Some extremist elements within incel groups have been linked to real-world violence, though most incels are nonviolent"a);
    io_print(io, "Research suggests that positive social support and therapy can reduce feelings of isolation among involuntary celibates"a);
    io_print(io, "Calls to bridge the gap focus on empathy, self-esteem building, and healthy communication skills"a);
}


// Insecurity: Add friends 
File& io = wt->io;
void insecurity(File & io) {
    // Start friend discovery
    io_print(io, "Scanning for available friends..."a);
    // Available friends with positivity signal values
    io_print(io, "1. Alex Wright - +8 positivity"a);
    io_print(io, "2. Bailey Hamm - +9 positivity"a);
    io_print(io, "3. Jordan Lee - +7 positivity"a);
    io_print(io, "4. Taylor Scott - +6 positivity"a);
    io_print(io, "5. Morgan Khan - +8 positivity"a);
    io_print(io, "6. Riley Patel - +5 positivity"a);
    io_print(io, "7. Casey Martin - +7 positivity"a);
    io_print(io, "8. Jamie Taylor - +6 positivity"a);
    io_print(io, "9. Eathan Tillman - +9 positivity"a);
    io_print(io, "10. Chris Rivera - +8 positivity"a);
    // Negative or challenging contacts with low/negative positivity
    io_print(io, "11. Patty Drama - -6 positivity"a);
    io_print(io, "12. Kevin Critic - -4 positivity"a);
    io_print(io, "13. Cindy Cynic - -5 positivity"a);
    io_print(io, "14. Bully Bob - -8 positivity"a);
    io_print(io, "15. Negan Nancy - -10 positivity"a);
    // Prompt to choose a friend or contact
    io_print(io, "Enter the number of the person you want to connect with..."a);
}

void insecurity_facts(File& io) {
	io_print(io, "Insecurity is a common human emotion that can stem from various sources."a);
	io_print(io, "It often involves feelings of self-doubt, inadequacy, and fear of rejection."a);
	io_print(io, "Insecurity can manifest in relationships, work, and social situations."a);
	io_print(io, "Building self-esteem and self-acceptance can help combat insecurity."a);
	io_print(io, "Seeking support from friends or professionals can also be beneficial."a);
}