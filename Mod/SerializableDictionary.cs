using System.Collections.Generic;
using System.Xml.Serialization;

namespace EnhancedGraphics;

// xml can't serialize dictionaries (lol) (lmao, even)
// see https://stackoverflow.com/questions/495647/serialize-class-containing-dictionary-member

[XmlRoot("dictionary")]
public class SerializableDictionary<TKey, TValue>
    : Dictionary<TKey, TValue>, IXmlSerializable {
    public SerializableDictionary() { }
    public SerializableDictionary(IDictionary<TKey, TValue> dictionary) : base(dictionary) { }
    public SerializableDictionary(IDictionary<TKey, TValue> dictionary, IEqualityComparer<TKey> comparer) : base(dictionary, comparer) { }
    public SerializableDictionary(IEqualityComparer<TKey> comparer) : base(comparer) { }
    public SerializableDictionary(int capacity) : base(capacity) { }
    public SerializableDictionary(int capacity, IEqualityComparer<TKey> comparer) : base(capacity, comparer) { }

    public System.Xml.Schema.XmlSchema GetSchema() {
        return null;
    }

    public void ReadXml(System.Xml.XmlReader reader) {
        XmlSerializer keySerializer = new(typeof(TKey));
        XmlSerializer valueSerializer = new(typeof(TValue));

        bool wasEmpty = reader.IsEmptyElement;
        reader.Read();

        if (wasEmpty) {
            return;
        }

        while (reader.NodeType != System.Xml.XmlNodeType.EndElement) {
            reader.ReadStartElement("item");

            reader.ReadStartElement("key");
            TKey key = (TKey)keySerializer.Deserialize(reader);
            reader.ReadEndElement();

            reader.ReadStartElement("value");
            TValue value = (TValue)valueSerializer.Deserialize(reader);
            reader.ReadEndElement();

            Add(key, value);

            reader.ReadEndElement();
            reader.MoveToContent();
        }

        reader.ReadEndElement();
    }

    public void WriteXml(System.Xml.XmlWriter writer) {
        XmlSerializer keySerializer = new(typeof(TKey));
        XmlSerializer valueSerializer = new(typeof(TValue));

        foreach (TKey key in Keys) {
            writer.WriteStartElement("item");

            writer.WriteStartElement("key");
            keySerializer.Serialize(writer, key);
            writer.WriteEndElement();

            writer.WriteStartElement("value");
            TValue value = this[key];
            valueSerializer.Serialize(writer, value);
            writer.WriteEndElement();

            writer.WriteEndElement();
        }
    }
}
