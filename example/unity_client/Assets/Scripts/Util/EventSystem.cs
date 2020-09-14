using UnityEngine;
using System.Collections.Generic;

namespace Util
{
    public class EventSystem : Singleton<EventSystem>
    {
		public static void Publish<T>(string eventID, T param) 
		{
			EventSystem.Instance._Publish<T>(eventID, param);
		}

        public static void Publish(string eventID)
        {
            EventSystem.Instance._Publish(eventID);
        }

        public static void Subscribe<T>(string eventID, System.Action<T> handler)
        {
            EventSystem.Instance._Subscribe<T>(eventID, handler);
        }

        public static void Subscribe(string eventID, System.Action handler) 
        {
            EventSystem.Instance._Subscribe(eventID, handler);
        }

        public static void Unsubscribe<T>(string eventID, System.Action<T> handler = null) 
        {
            EventSystem.Instance._Unsubscribe<T>(eventID, handler);
        }

        public static void Unsubscribe(string eventID, System.Action handler = null)
        {
            EventSystem.Instance._Unsubscribe(eventID, handler);
        }

        public abstract class IEventHandler
        {
            public abstract void OnEvent(object eventParam);
        }

        public class EventHandler<T> : IEventHandler
        {
            public System.Action<T> onEvent;

            public override void OnEvent(object eventParam)
            {
                if (null == onEvent)
                {
                    throw new System.Exception("no event handler");
                }

                T param = (T)eventParam;
				onEvent(param);
            }
        }

        public class EventHandler : IEventHandler 
        {
            public System.Action onEvent;

            public override void OnEvent(object eventParam)
            {
                if (null == onEvent)
                {
                    throw new System.Exception("no event handler");
                }

                onEvent();
            }
        }

        private Dictionary<string, IEventHandler> event_handlers = new Dictionary<string, IEventHandler>();

        private void _Subscribe<T>(string eventID, System.Action<T> handler) 
        {
            EventHandler<T> eventHandler = null;
            if (true == event_handlers.ContainsKey(eventID))
            {
                try
                {
                    eventHandler = (EventHandler<T>)event_handlers[eventID];
                }
                catch(System.InvalidCastException)
                {
                    throw new System.Exception("try to subscribe same event(event_id:" + eventID + ") but different callback function proto type(System.Action<" + typeof(T).FullName + ">)");
                }
            }
            else
            {
                eventHandler = new EventHandler<T>();
                event_handlers.Add(eventID, eventHandler);
            }
            eventHandler.onEvent += handler;
        }

        private void _Subscribe(string eventID, System.Action handler)
        {
            EventHandler eventHandler = null;
            if (true == event_handlers.ContainsKey(eventID))
            {
                try
                { 
                    eventHandler = (EventHandler)event_handlers[eventID];
                }
                catch (System.InvalidCastException)
                {
                    throw new System.Exception("try to subscribe same event(event_id:" + eventID + ") but different callback function proto type(System.Action)");
                }
            }
            else
            {
                eventHandler = new EventHandler();
                event_handlers.Add(eventID, eventHandler);
            }
            eventHandler.onEvent += handler;
        }

        private void _Unsubscribe<T>(string eventID, System.Action<T> handler)
        {
            if (false == event_handlers.ContainsKey(eventID))
            {
                Debug.LogWarning("can not find event key(event_id:" + eventID + ")");
                return;
            }

            EventHandler<T> eventHandler = (EventHandler<T>)event_handlers[eventID];
            if (null != handler)
            {
                eventHandler.onEvent -= handler;
            }
            else
            {
                eventHandler.onEvent = null;
            }

            if (null == eventHandler.onEvent)
            {
                event_handlers.Remove(eventID);
            }
        }

        private void _Unsubscribe(string eventID, System.Action handler) 
        {
            if (false == event_handlers.ContainsKey(eventID))
            {
                Debug.LogWarning("can not find event key(event_id:" + eventID + ")");
                return;
            }

            EventHandler eventHandler = (EventHandler)event_handlers[eventID];
            if (null != handler)
            {
                eventHandler.onEvent -= handler;
            }
            else
            {
                eventHandler.onEvent = null;
            }

            if (null == eventHandler.onEvent)
            {
                event_handlers.Remove(eventID);
            }
        }

		private void _Publish<T>(string eventID, T param) 
		{
			if (false == event_handlers.ContainsKey(eventID))
			{
				Debug.LogWarning("can not find event key(event_id:" + eventID + ")");
				return;
			}

			event_handlers[eventID].OnEvent(param);
		}
		private void _Publish(string eventID)
        {
            if (false == event_handlers.ContainsKey(eventID))
            {
                Debug.LogWarning("can not find event key(event_id:" + eventID + ")");
                return;
            }

            event_handlers[eventID].OnEvent(null);
        }
    }
}