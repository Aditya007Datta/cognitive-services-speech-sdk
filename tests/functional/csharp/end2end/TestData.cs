//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System.IO;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    static class TestData
    {
        public static string AudioDir { get; set; }

        public static string KwsDir { get; set; }

        public static class Kws
        {
            public static class Computer
            {
                public static readonly string ModelFile = Path.Combine(KwsDir, Path.Combine("Computer", "kws.table"));
                public static readonly string ModelKeyword = "computer";

                public static readonly string AudioFile = Path.Combine(KwsDir, "kws_whatstheweatherlike.wav");
                public static readonly string AudioFile2x = Path.Combine(KwsDir, "kws_whatstheweatherlike_x2.wav");
                public static readonly string Utterance= "Computer what's the weather like?";
            }
            public static class Computer2
            {
                public static readonly string ModelFile = Path.Combine(KwsDir, Path.Combine("Computer", "kws.table"));
                public static readonly string ModelKeyword = "computer";

                public static readonly string AudioFile = Path.Combine(KwsDir, "kws_whatstheweatherlike_howoldareyou.wav");
                public static readonly string AudioFileIntent = Path.Combine(KwsDir, "kws_whatstheweatherlike_howoldareyou_intent.wav");
                public static readonly string AudioFileTranslate = Path.Combine(KwsDir, "kws_whatstheweatherlike_howoldareyou_translate.wav");
                public static readonly string Utterance1 = "Computer what's the weather like?";
                public static readonly string Utterance2 = "Computer how old are you?";
            }

            public static class Secret
            {
                public static readonly string ModelFile = Path.Combine(KwsDir, Path.Combine("Secret", "kws.table"));
                public static readonly string ModelKeyword = "secret";

                public static readonly string AudioFile = Path.Combine(KwsDir, "kws_secret.wav");
                public static readonly string AudioFile2x = Path.Combine(KwsDir, "kws_secret_x2.wav");
                public static readonly string Utterance = "Secret what's the weather like?";
            }
        }

        public static class German
        {
            public static class Batman
            {
                public static readonly string[] Utterances =
                {
                    "Fähigkeiten und Fähigkeiten Batman hat keine inhärenten Superkräfte. Er stützt sich auf seine eigenen wissenschaftlichen Fähigkeiten Detektiv-Fähigkeiten und sportliche Fähigkeiten in den Geschichten Batman gilt als einer der weltweit größten Detektiv, wenn nicht der größte Kriminallöser der Welt wurde immer wieder als Genie beschrieben. Level-Intellekt.",
                    "Die größten Kampfkünstler im DC-Universum und mit einem Höhepunkt der menschlichen körperlichen Konditionierung Fahrt bereiste die Welt und erwarb die Fähigkeiten, die notwendig waren, um seine Cruceta zu helfen, die Verbrechen.",
                    "Sein Wissen und seine Expertise in fast jeder Disziplin, die dem Menschen bekannt ist, ist von keinem anderen Charakter im Universum beispiellos, dass der unerschöpfliche Reichtum des Menschen ihm erlaubt, auf fortschrittliche Technologie zuzugreifen, da ein erfahrener Wissenschaftler in der Lage ist, zu verwenden und zu modifizieren. Diese Technologien zu seinem Vorteil Batman.",
                    "Vybz Superman als der gefährlichste Mann der Welt in der Lage, eine Mannschaft von super angetriebenen Außerirdischen von sich selbst zu besiegen, um seine inhaftierten Teamkollegen zu retten.",
                    "In der ersten Storyline Superman hält auch Batman als einer der brillantesten Köpfe auf dem Planeten.",
                    "Batman hat die Fähigkeit, unter großen körperlichen Schmerzen zu funktionieren und der Gedankenkontrolle standzuhalten. Er ist ein Meister der Verschleierung mehrsprachig und ein Experte für Spionage, der oft Informationen unter verschiedenen Identitäten sammelt.",
                    "Batman es Karate Judo und Jujitsu Ausbildung hat ihn zu einem Meister der Verstohlenheit und Flucht gemacht, die es ihm ermöglicht, zu erscheinen und nach Belieben zu verschwinden und sich zu befreien.",
                    ""
                };
            }

            public static class HowIsYourWork
            {
                public const string Utterance = "Wie machen Sie heute mit den Arbeitsstücken";
            }

            public static class Weather
            {
                public static readonly string Utterance = "Wie ist das Wetter?";
            }

            public static class FirstOne
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "CallTheFirstOne.wav");
                public static readonly string Utterance = "Rufe die erste an.";
            }
        }

        public static class Chinese
        {
            public static class Weather
            {
                public static readonly string Utterance = "天气怎么样？";
            }
        }

        public static class English
        {
            public static class Margarita
            {
                // Manual transcription:
                // <music at the beginning, continues in the background once
                // speaker starts around second 7.25> we all know and love a
                // margarita and if you're anything like me you've probably had
                // your fair share and if you have you've probably noticed that
                // margaritas can vary from incredible to <ugh> so I'm gonna
                // show you how to make one of those incredibly fresh and
                // delicious margaritas there are 3 key steps you need to
                // follow first
                public static readonly string AudioFile = Path.Combine(AudioDir, "Margarita-44100.wav");
                public static readonly string[] Utterance =
                {
                    "And we all know and Love, a margarita and if you're anything like me, you probably had your fair share and if you have youve probably noticed that margaritas can vary from incredible 2?",
                    "Yeah, so I'm going to show you how to make one of those incredibly fresh and Delicious Margaritas. There are 3 key steps you need to follow first."

                };
            }

            public static class Batman
            {
                public static readonly string AudioFile = Path.Combine(AudioDir,"batman.wav");
                // This is expected results using StartContinuousRecognitionAsync() which uses converstation mode.
                // The result using RecognizeOnceAsync() might be slightly different, since it uses interactive mode.
                public static readonly string[] Utterances =
                {
                    "Skills and abilities Batman has no inherent super powers, he relies on his own scientific knowledge detective skills and athletic prowess in the stories. Batman is regarded as one of the world's greatest detective if not the world's greatest crime solver. Batman has been repeatedly described as having genius level intellect.",
                    "Greatest martial artists in the DC universe and having peak human physical conditioning use traveled the world, acquiring the skills needed to aid his cruceta grants crime.",
                    "His knowledge and expertise in almost every discipline known to man is nearly unparalleled by any other character in the universe. That man's inexhaustible wealth allows him to access advanced technology as a proficient scientist is able to use and modify those technologies to his advantage Batman.",
                    "Vybz Superman as the most dangerous man on Earth able to defeat a team of super powered extraterrestrials by himself in order to rescue his imprison teammates.",
                    "In the first storyline, Superman also considers Batman to be one of the most brilliant minds on the planet.",
                    "Batman has the ability to function under great physical pain and withstand mind control. He is a master of disguise multilingual and an expert in espionage, often gathering information under different identity's.",
                    "Batman's Karate Judo and jujitsu training has made him a math are master of stealth an escape, allowing allowing him to appear and disappear at Will and to break free.",
                    ""
                };

                public static readonly string[] UtterancesInteractive =
                {
                    "Skills and abilities Batman has no inherent super powers, he relies on his own scientific knowledge detective skills and athletic prowess in the stories. Batman is regarded as one of the world's greatest detective.",
                    "If not the world's greatest crime solver. Batman has been repeatedly described as having genius level intellect. One of the greatest martial artists in the DC universe and having peak human physical conditioning, He.",
                    "He's traveled the world, acquiring the skills needed to aid his cruceta grants crime. His knowledge and expertise in almost every discipline known to man is nearly unparalleled by any other character in the universe Batman's inex.",
                    "Possible wealth allows him to access advanced technology as a proficient scientists use able to use and modify those technologies to his advantage. Batman describes Superman as the most dangerous man.",
                    "Earth able to defeat a team of Super powered extraterrestrials by himself in order to rescue his imprison teammates in the.",
                    "For a storyline Superman also considers Batman to be one of the most brilliant minds on the planet Batman.",
                    "Has the ability to function under great physical pain and withstand mind control. He is a master of disguise multilingual and an expert in espionage, often gathering information under different I dent.",
                    "Tis Batman,",
                    "'s Karate Judo and jujitsu training has made him a math master of stealth an escape, allowing allowing him to appear and disappear at Will and to break free.",
                    ""
                };

                // Normalization for translation may be different.
                public static readonly string[] UtterancesTranslation =
                {
                    "Skills and abilities Batman has no inherent super powers. He relies on his own scientific knowledge detective skills and athletic prowess in the stories batman is regarded as one of the world's greatest detective if not the world's Greatest Crime Solver Batman has been repeatedly described as having genius. Level intellect.",
                    "Greatest martial artists in the DC universe and having peak human physical conditioning use traveled the world acquiring the skills needed to aid his cruceta grants crime.",
                    "His knowledge and expertise in almost every discipline known to man is nearly unparalleled by any other character in the universe that man's inexhaustible wealth allows him to access advanced technology as a proficient scientist is able to use and modify. Those technologies to his advantage batman.",
                    "Vybz superman as the most dangerous man on earth able to defeat a team of super powered extraterrestrials by himself in order to rescue his imprison teammates.",
                    "In the first storyline superman also considers batman to be one of the most brilliant minds on the planet.",
                    "Batman has the ability to function under great physical pain and withstand mind control. He is a master of disguise multilingual and an expert in espionage often gathering information under different identity's.",
                    "Batman's karate judo and jujitsu training has made him a math are master of stealth an escape allowing him to appear and disappear at will and to break free.",
                    ""
                };
            }

            public static class Weather
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "whatstheweatherlike.wav");
                public static readonly string Utterance = "What's the weather like?";
            }

            public static class Silence
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "silenceshort.wav");
            }
            public static class HomeAutomation
            {
                public static class TurnOn
                {
                    public static readonly string AudioFile = Path.Combine(AudioDir, "TurnOnTheLamp.wav");
                    public static readonly string Utterance = "Turn on the lamp.";
                }
                public static class TurnOff
                {
                    public static readonly string AudioFile = Path.Combine(AudioDir, "ShutDownTheComputer.wav");
                    public static readonly string Utterance = "Shut down the computer.";
                }
            }

            public static class TStockValue
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "TStockValue.wav");
                public static readonly string Utterance = "What's the AT&T stock value today?";
            }

            public static class DgiWreckANiceBeach
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "wreck-a-nice-beach.wav");
                public static readonly string CorrectRecoText = "Wreck a nice beach.";
                public static readonly string DefaultRecoText = "Recognize speech.";
            }
        }

        public static class French
        {
            public static class Batman
            {
                public static readonly string[] Utterances =
                {
                    "Compétences et aptitudes Batman n'a pas de super pouvoirs inhérents. Il s'appuie sur ses propres connaissances scientifiques des compétences de détective et des prouesses athlétiques."
                };
            }

            public static class FirstOne
            {
                public static readonly string Utterance = "Appelez le premier.";
            }

            public static class Weather
            {
                public static readonly string Utterance = "Quel temps fait-il?";
            }
        }

        public static class Spanish
        {
            public static class Batman
            {
                public static readonly string[] Utterances =
                {
                    "Habilidades y habilidades Batman no tiene súper poderes inherentes. Se basa en su propio conocimiento científico habilidades de detective y destreza atlética."
                };
            }

            public static class FirstOne
            {
                public static readonly string[] PossibleUtterances =
                {
                    "Llama al primero.",
                    "Llama a la primera."
                };
            }

            public static class Weather
            {
                public static readonly string[] PossibleUtterances =
                {
                    "¿Cómo es el clima?",
                    "¿Cómo es el tiempo?"
                };
            }
        }

        public static class Catalan
        {
            public static readonly string AudioFile = Path.Combine(AudioDir, "ca-es.wav");

            public static class HowIsYourWork
            {
                public static readonly string Utterance = @"Com estàs fent amb els elements de treball avui";
            }

            public static class Weather
            {
                public static readonly string Utterance = "Wie ist das Wetter?";
            }

            public static class FirstOne
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "CallTheFirstOne.wav");
                public static readonly string Utterance = "Rufe die erste an.";
            }
        }

        public static class ExpectedErrorDetails
        {
            public static string InvalidTargetLanaguageErrorMessage = @"Translation call failed: Bad Request - Unsuccessful call to the translation service: {""error"":{""code"":400036,""message"":""The target language is not valid.""}}";
            public static string InvalidVoiceNameErrorMessage = @"Synthesis service failed with code:  - Could not identify the voice 'InvalidVoice' for the text to speech service ";
        }
    }
}