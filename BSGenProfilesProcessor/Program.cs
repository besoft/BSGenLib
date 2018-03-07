using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;
using System.Text.RegularExpressions;
using Microsoft.Office.Interop.Excel;


namespace BSGenProfilesProcessor
{
    internal class ProfileVar
    {
        public string ProfileId;          
        public string Name;
        public string SourceFile;
        public string ThreadId;

        public string StringValue;
        public double Value;            ///NaN for values unable to convert to double        
        public int RunCount;            ///0 for Counters
    }

    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                Console.WriteLine("This utility processes the given BSGenLib profile_log.xml file or\nall profile files from the given directory\nand produces .xlsx file containing the measured data.\n\nSyntax: BSGenProfilesProcessor <pathname>");
            }
            else
                new Program().Run(args[0]);
        }

        Dictionary<string, List<ProfileVar>> ProfileVars = new Dictionary<string, List<ProfileVar>>();
        
        private void Run(string pathname)
        {
            string[] files = GetProfileFiles(pathname);

            foreach (var s in files)
            {
                LoadProfile(s);
            }
            
            //Sort ProfileVars by their names and for each profile variable order the data by ProfileId
            string[] keys = this.ProfileVars.Keys.ToArray();
            Array.Sort(keys);

            List<ProfileVar>[] pvars = new List<ProfileVar>[keys.Length];
            for (int i = 0; i < keys.Length; i++)
            {
                pvars[i] = this.ProfileVars[keys[i]];
                pvars[i].Sort((x, y) => x.ProfileId.CompareTo(y.ProfileId));
            }

            var xlsApp = new Application();
            xlsApp.Visible = true;
            var xlsWbk = xlsApp.Workbooks.Add();

            Worksheet xlsSheet2 = xlsWbk.Worksheets.Add();
            xlsSheet2.Name = Path.GetFileNameWithoutExtension(pathname) + "_COUNTS";

            Worksheet xlsSheet = xlsWbk.Worksheets.Add();
            xlsSheet.Name = Path.GetFileNameWithoutExtension(pathname) + "_VALUES";            
            

            Dictionary<string, int> profIdToRow = new Dictionary<string, int>();
            
            //Header
            xlsSheet2.Cells[1, 1] = xlsSheet.Cells[1, 1] = "ProfileId";
            for (int i = 0, cell = 2; i < keys.Length; i++, cell++)
            {
                xlsSheet2.Cells[1, cell] = xlsSheet.Cells[1, cell] = keys[i];
                
                //populate data
                for (int j = 0; j < pvars[i].Count; j++)
                {
                    var pvar = pvars[i][j];

                    if (!profIdToRow.ContainsKey(pvar.ProfileId))
                        profIdToRow.Add(pvar.ProfileId, profIdToRow.Count() + 2);

                    int row = profIdToRow[pvar.ProfileId];
                    if (Double.IsNaN(pvar.Value))
                        xlsSheet.Cells[row, cell] = pvar.StringValue;
                    else 
                        xlsSheet.Cells[row, cell] = pvar.Value;

                    xlsSheet2.Cells[row, cell] = pvar.RunCount;

                    xlsSheet2.Cells[row, 1] = xlsSheet.Cells[row, 1] = pvar.ProfileId;
                }
            }

            


            //xlsApp.Quit();
        }

        private void LoadProfile(string pathname)
        {
            string profileId = Path.GetFileNameWithoutExtension(pathname);

            var doc = new XmlDocument();
            doc.Load(pathname);

            LoadProfileVars(doc, "//item[@name='Counters']", profileId);
            LoadProfileVars(doc, "//item[@name='Functions']", profileId);
            LoadProfileVars(doc, "//item[@name='Timers']", profileId);                              
        }

        private void LoadProfileVars(XmlDocument doc, string xpath, string profileId)
        {
            var root = doc.SelectSingleNode(xpath);
            if (root != null)
            {
                foreach (XmlNode node in root.SelectNodes("item"))
                {
                    ProfileVar pvar = CreateProfileVar(node as XmlElement, profileId);

                    if (!this.ProfileVars.ContainsKey(pvar.Name))
                        this.ProfileVars.Add(pvar.Name, new List<ProfileVar>());

                    ProfileVars[pvar.Name].Add(pvar);
                }
            }    
        }

        private ProfileVar CreateProfileVar(XmlElement xmlElement, string profileId)
        {
            ProfileVar ret = new ProfileVar();
            ret.ProfileId = profileId;
            ret.Name = xmlElement.GetAttribute("name");

            //parse name and extract source file
            var rexp = new Regex(@"([^\(@\t]*)\t*(\([^\)]*\))?(@THREAD_\S*)?");
            Match mtch = rexp.Match(ret.Name);            
            ret.Name = mtch.Groups[1].Value;
            ret.SourceFile = mtch.Groups[2].Success ? mtch.Groups[2].Value : null;
            ret.ThreadId = mtch.Groups[3].Success ? mtch.Groups[3].Value : null; 
            
            ret.StringValue = xmlElement.GetAttribute("value");

            //check subitems
            XmlElement elTotalTime = xmlElement.SelectSingleNode("subitem[@name='TotalTime']") as XmlElement;
            XmlElement elCount = xmlElement.SelectSingleNode("subitem[@name='Count']") as XmlElement;

            if (elTotalTime != null)
                ret.StringValue = elTotalTime.GetAttribute("value");

            if (elCount != null)
                ret.RunCount = Convert.ToInt32(elCount.GetAttribute("value"));
            else
                ret.RunCount = elTotalTime == null ? 0 : 1;

            if (!Double.TryParse(ret.StringValue, out ret.Value))
                ret.Value = Double.NaN;

            return ret;            
        }
        
        private string[] GetProfileFiles(string pathname)
        {
            if (File.Exists(pathname))
                return new string[1]{pathname};

            return Directory.EnumerateFiles(pathname, "*.xml").ToArray();            
        }
    }
}
