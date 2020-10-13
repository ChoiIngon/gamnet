
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;

public class CSVTable : IEnumerable
{
	private Dictionary<string, int> column_name_to_index = null;
	private List<string> column_names = null;
	private List<List<string>> rows = null;

	public class Row
	{
		public Row(CSVTable table, List<string> row)
		{
			this.table = table;
			this.row = row;
		}

		public string GetValue(string columnName)
		{
			return row[table.GetIndex(columnName.ToLower())];
		}

		private List<string> row;
		private CSVTable table;
	}

	public void ReadStream(string stream)
	{
		column_name_to_index = new Dictionary<string, int>();
		rows = new List<List<string>>();
		string[] data = stream.Split('\n');
		char[] trimChar = "\r\"".ToCharArray();
		string[] names = data[0].Trim(trimChar).Split(',');
		column_names = new List<string>(names);
		string[] types = data[1].Trim(trimChar).Split(',');

		for (int i = 0; i < names.Length; ++i)
		{
			if (true == String.IsNullOrEmpty(names[i]))
			{
				throw new System.Exception("error");
			}

			names[i] = names[i].Trim(trimChar).ToLower();
			column_name_to_index.Add(names[i], i);
		}

		for (int i = 3; i < data.Length; ++i)
		{
			List<string> row = new List<string>(data[i].Trim(trimChar).Split(','));
			if (names.Length != row.Count)
			{
				continue;
			}
			for (int j = 0; j < row.Count; ++j)
			{
				row[j] = row[j].Trim(trimChar);
			}
			rows.Add(row);
		}
	}

	public void OpenFile(string fileName)
	{
		FileStream file = File.Open(fileName, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);

		byte[] buffer = new byte[file.Length];
		file.Read(buffer, 0, buffer.Length);
		file.Close();

		string text = System.Text.Encoding.Default.GetString(buffer, 0, buffer.Length);
		ReadStream(text);
	}

	public int GetIndex(string columnName)
	{
		if (false == column_name_to_index.ContainsKey(columnName))
		{
			return -1;
		}

		return column_name_to_index[columnName];
	}

	public List<string> GetColumnNames()
	{
		return column_names;
	}

	public Row GetRow(int index)
	{
		if (0 > index || rows.Count <= index)
		{
			throw new System.IndexOutOfRangeException();
		}

		Row row = new Row(this, rows[index]);
		return row;
	}

	public int GetRowCount()
	{
		return rows.Count;
	}

	IEnumerator IEnumerable.GetEnumerator()
	{
		for (int i = 0; i < rows.Count; i++)
		{
			yield return GetRow(i);
		}
	}
}



