using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

class MetaData
{
	private Dictionary<string, Action<Object>> bind_functions = new Dictionary<string, Action<object>>();	

	public bool OnLoad()
	{
		return true;
	}

	protected void Bind(string name, ref bool member)
	{
		/*
		bind_functions.Add(name.ToLower(), (Object obj) =>
		{
			string value = obj["value"].asString();
			if ("false" == boost::algorithm::to_lower_copy(value))
			{
				member = false;
				return;
			}
			else if ("true" == boost::algorithm::to_lower_copy(value))
			{
				member = true;
				return;
			}
		});
		*/
	}
}

class MetaReader<T> where T : MetaData, new()
{
	private List<T> meta_datas;

	List<T> Read(string filePath)
	{
		meta_datas = new List<T>();
		CSVTable table = new CSVTable();
		table.OpenFile(filePath);
		foreach (CSVTable.Row row in table)
		{

		}
		return null;
	}
}
